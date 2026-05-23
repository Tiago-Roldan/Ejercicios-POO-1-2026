from fastapi import FastAPI, HTTPException, Header
from sqlalchemy import create_engine, Column, Integer, String, ForeignKey
from sqlalchemy.orm import sessionmaker, declarative_base, relationship
import os
import requests

DATABASE_URL = os.getenv("DATABASE_URL")

engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(bind=engine)
Base = declarative_base()

app = FastAPI()

# ======================
# MODELOS
# ======================

class ColumnModel(Base):
    __tablename__ = "columns"
    id = Column(Integer, primary_key=True)
    title = Column(String(255))
    position = Column(Integer)

class CardModel(Base):
    __tablename__ = "cards"
    id = Column(Integer, primary_key=True)
    title = Column(String(255))
    description = Column(String(500))
    column_id = Column(Integer, ForeignKey("columns.id"))

class CardOrder(Base):
    __tablename__ = "card_order"
    id = Column(Integer, primary_key=True)
    card_id = Column(Integer, ForeignKey("cards.id"))
    position = Column(Integer)

Base.metadata.create_all(bind=engine)

# ======================
# AUTH
# ======================

AUTH_API_URL = os.getenv("AUTH_API_URL")

def verify_token(token: str):
    try:
        r = requests.get(
            f"{AUTH_API_URL}/verify",
            params={"token": token},
            timeout=2
        )

        #Para depuracion
        print("STATUS:", r.status_code)
        print("RESPONSE:", r.text)

        if r.status_code != 200:
            return False

        data = r.json()
        return data.get("valid", False)

    except Exception as e:
        print("ERROR VERIFY:", e)
        return False

def get_user(authorization: str = Header(...)):
    token = authorization.split(" ")[1]
    if not verify_token(token):
        raise HTTPException(status_code=401, detail="Invalid token")

# ======================
# ENDPOINTS
# ======================

@app.get("/board")
def get_board(authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()

    columns = db.query(ColumnModel).order_by(ColumnModel.position).all()
    result = []

    for col in columns:
        cards = (
            db.query(CardModel, CardOrder)
            .join(CardOrder, CardModel.id == CardOrder.card_id)
            .filter(CardModel.column_id == col.id)
            .order_by(CardOrder.position)
            .all()
        )

        result.append({
            "id": col.id,
            "title": col.title,
            "position": col.position,
            "cards": [
                {
                    "id": c.CardModel.id,
                    "title": c.CardModel.title,
                    "description": c.CardModel.description,
                    "position": c.CardOrder.position
                }
                for c in cards
            ]
        })

    return {"columns": result}


@app.post("/columns")
def create_column(data: dict, authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()
    col = ColumnModel(title=data["title"], position=0)
    db.add(col)
    db.commit()
    return {"message": "created"}


@app.put("/columns/{column_id}")
def update_column(column_id: int, data: dict, authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()

    column = db.query(ColumnModel).filter_by(id=column_id).first()
    if not column:
        raise HTTPException(status_code=404, detail="Column not found")

    if "title" in data:
        column.title = data["title"]

    db.commit()

    return {"message": "updated"}


@app.delete("/columns/{column_id}")
def delete_column(column_id: int, authorization: str = Header(...)):
    get_user(authorization)
    db = SessionLocal()

    try:
        column = db.query(ColumnModel).filter_by(id=column_id).first()
        if not column:
            raise HTTPException(status_code=404, detail="Column not found")

        cards = db.query(CardModel).filter_by(column_id=column_id).all()

        for card in cards:
            # 1. borrar fila en card_order
            order = db.query(CardOrder).filter_by(card_id=card.id).first()
            if order:
                db.delete(order)
                db.flush()

            # 2. borrar fila en cards
            db.delete(card)
            db.flush()

        # 3. borrar fila en columns
        db.delete(column)
        db.flush()

        db.commit()
        return {"message": "deleted"}

    except HTTPException:
        db.rollback()
        raise
    except Exception as e:
        db.rollback()
        print("DELETE COLUMN ERROR:", e)
        raise HTTPException(status_code=500, detail=f"Delete column failed: {str(e)}")

    finally:
        db.close()


@app.post("/cards")
def create_card(data: dict, authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()

    card = CardModel(
        title=data["title"],
        description=data["description"],
        column_id=data["column_id"]
    )

    db.add(card)
    db.commit()
    db.refresh(card)

    order = CardOrder(card_id=card.id, position=0)
    db.add(order)
    db.commit()

    return {"message": "created"}


@app.put("/cards/{card_id}")
def update_card(card_id: int, data: dict, authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()

    card = db.query(CardModel).filter_by(id=card_id).first()
    if not card:
        raise HTTPException(status_code=404, detail="Card not found")

    if "title" in data:
        card.title = data["title"]
    if "description" in data:
        card.description = data["description"]
    if "column_id" in data:
        card.column_id = data["column_id"]

    db.commit()

    return {"message": "updated"}


@app.delete("/cards/{card_id}")
def delete_card(card_id: int, authorization: str = Header(...)):
    get_user(authorization)
    db = SessionLocal()

    try:
        card = db.query(CardModel).filter_by(id=card_id).first()
        if not card:
            raise HTTPException(status_code=404, detail="Card not found")

        # 1. borrar fila en card_order
        order = db.query(CardOrder).filter_by(card_id=card_id).first()
        if order:
            db.delete(order)
            db.flush()

        # 2. borrar fila en cards
        db.delete(card)
        db.flush()

        db.commit()
        return {"message": "deleted"}

    except HTTPException:
        db.rollback()
        raise
    except Exception as e:
        db.rollback()
        print("DELETE CARD ERROR:", e)
        raise HTTPException(status_code=500, detail=f"Delete card failed: {str(e)}")

    finally:
        db.close()


@app.post("/cards/{card_id}/move")
def move_card(card_id: int, data: dict, authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()

    card = db.query(CardModel).filter_by(id=card_id).first()
    if not card:
        raise HTTPException(status_code=404, detail="Card not found")

    card.column_id = data["to_column_id"]

    order = db.query(CardOrder).filter_by(card_id=card.id).first()
    if not order:
        raise HTTPException(status_code=404, detail="Card order not found")

    order.position = data["new_position"]

    db.commit()

    return {"message": "moved"}


@app.post("/cards/{card_id}/reorder")
def reorder(card_id: int, data: dict, authorization: str = Header(...)):
    get_user(authorization)

    db = SessionLocal()

    card = db.query(CardModel).filter_by(id=card_id).first()
    if not card:
        raise HTTPException(status_code=404, detail="Card not found")

    order = db.query(CardOrder).filter_by(card_id=card_id).first()
    if not order:
        raise HTTPException(status_code=404, detail="Card order not found")

    order.position = data["new_position"]

    db.commit()

    return {"message": "reordered"}