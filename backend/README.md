# Set up for developers (App frontend)

1. Enter into `./backend` directory.
2. Paste following commands:

- `py -3.12 -m venv .venv`
- `. .venv/Scripts/activate`
- `pip install -r requirements.txt`

3. Enter into `./backend/app/computing`

- Run script: `python build_pyd.py`

4. Application backend has been installed correctly, you can now move back and run app: [Main manual](../README.md).

### Tips

1. If you want to compile C++ code into Python `pyd` package follow the next steps:

- Enter into: `./backend` directory.
- Activate virtual venv: `. .venv/Scripts/activate`
- Enter into: `./backend/app/computing`
- Run script: `python build_pyd.py`

2. If you want to only run application backend follow the next steps:

- Enter into `./backend` direcotry.
- Run backend script: `python main.py`
