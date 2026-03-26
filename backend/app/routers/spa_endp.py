from pathlib import Path

from fastapi import Request, APIRouter
from fastapi.responses import FileResponse

router = APIRouter()

BASE_DIR = Path(__file__).resolve()
frontend_path = BASE_DIR.parents[3] / "frontend" / "dist"


@router.get("/{full_path:path}")
async def serve_spa(full_path: str, request: Request):
    if full_path.startswith("api"):
        return {"detail": "Not Found"}

    file_path = frontend_path / full_path
    if file_path.exists() and file_path.is_file():
        return FileResponse(file_path)

    return FileResponse(frontend_path / "index.html")
