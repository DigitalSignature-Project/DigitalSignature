from pathlib import Path
import sys
import os

from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware

from app.routers import spa_endp as spa_router
from app.routers import crypto_endp as crypto_router
from app.routers import external_server_endp as server_router

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

if getattr(sys, "frozen", False):
    base_path = os.path.join(sys._MEIPASS, "frontend", "dist")
else:
    base_path = Path(__file__).resolve().parents[2] / "frontend" / "dist"

app.mount("/static", StaticFiles(directory=str(base_path), html=True), name="frontend")

app.include_router(spa_router.router)
app.include_router(crypto_router.router, prefix="/api")
app.include_router(server_router.router, prefix="/server")
