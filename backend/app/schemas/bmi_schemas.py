from pydantic import BaseModel


class Bmi(BaseModel):
    weight: int
    height: int


class BmiResult(BaseModel):
    result: float
