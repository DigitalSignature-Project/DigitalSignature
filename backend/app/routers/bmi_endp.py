from fastapi import APIRouter

from app.schemas.bmi_schemas import Bmi, BmiResult
from app.computing.dist import bmi_computing

router = APIRouter()


@router.post("/bmi", response_model=BmiResult)
async def calculate_bmi(data: Bmi) -> BmiResult:
    # bmi_value = bmi_computing.calculate_bmi(data.height, data.weight)
    bmi_value = bmi_computing.add_two_numbers(data.height, data.weight)
    return BmiResult(result=bmi_value)
