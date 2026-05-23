from fastapi import FastAPI
import psutil
import time

app = FastAPI()

def format_uptime(seconds):
    days = int(seconds // (24 * 3600))
    seconds %= (24 * 3600)
    hours = int(seconds // 3600)
    seconds %= 3600
    minutes = int(seconds // 60)
    seconds = int(seconds % 60)

    return f"{days} days {hours:02}:{minutes:02}:{seconds:02}"

@app.get("/health")
def health():
    # uptime
    boot_time = psutil.boot_time()
    uptime_seconds = time.time() - boot_time

    # load average (solo funciona bien en Linux)
    load = psutil.getloadavg()[0]

    # memoria (en MB)
    memory = psutil.virtual_memory()
    mem_used = int(memory.used / (1024 * 1024))
    mem_total = int(memory.total / (1024 * 1024))

    # disco (en GB)
    disk = psutil.disk_usage('/')
    disk_used = int(disk.used / (1024 * 1024 * 1024))
    disk_total = int(disk.total / (1024 * 1024 * 1024))

    return {
        "status": "OK",
        "uptime": format_uptime(uptime_seconds),
        "load": round(load, 2),
        "mem_used": mem_used,
        "mem_total": mem_total,
        "disk_used": disk_used,
        "disk_total": disk_total
    }