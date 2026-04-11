from fastapi import FastAPI
from pydantic import BaseModel
from typing import List, Dict
import json
import os

app = FastAPI()

STORAGE_FILE = "storage.json"

# ------------------------
# MODELOS
# ------------------------

class Point(BaseModel):
    x: float
    y: float

class Color(BaseModel):
    r: int
    g: int
    b: int
    a: int

class Stroke(BaseModel):
    id: str
    width: int
    eraser: bool
    color: Color
    points: List[Point]

class Drawing(BaseModel):
    strokes: List[Stroke]

# ------------------------
# STORAGE EN MEMORIA
# ------------------------

strokes_dict: Dict[str, dict] = {}

# ------------------------
# CARGA INICIAL
# ------------------------

def load_storage():
    global strokes_dict
    if os.path.exists(STORAGE_FILE):
        with open(STORAGE_FILE, "r") as f:
            data = json.load(f)
            strokes_dict = {s["id"]: s for s in data.get("strokes", [])}

def save_storage():
    with open(STORAGE_FILE, "w") as f:
        json.dump({"strokes": list(strokes_dict.values())}, f, indent=2)

load_storage()

# ------------------------
# ENDPOINTS
# ------------------------

@app.get("/drawing")
def get_drawing():
    return {"strokes": list(strokes_dict.values())}

@app.post("/drawing")
def add_strokes(drawing: Drawing):
    added = 0

    for stroke in drawing.strokes:
        if stroke.id not in strokes_dict:
            strokes_dict[stroke.id] = stroke.dict()
            added += 1

    save_storage()

    return {
        "status": "ok",
        "added": added,
        "total": len(strokes_dict)
    }