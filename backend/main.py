import uvicorn

from app.run import app

if __name__ == "__main__":
    # uvicorn.run("app.run:app", host="127.0.0.1", port=2138, reload=True)
    uvicorn.run(app, host="127.0.0.1", port=2138, log_config=None)
