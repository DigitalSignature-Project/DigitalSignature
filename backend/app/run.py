from pathlib import Path

from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware

from app.routers import spa_endp as spa_router
from app.routers import crypto_endp as crypto_router

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

frontend_dist = Path(__file__).resolve().parents[2] / "frontend" / "dist"
app.mount(
    "/static", StaticFiles(directory=str(frontend_dist), html=True), name="frontend"
)


app.include_router(spa_router.router)
app.include_router(crypto_router.router, prefix="/api")
