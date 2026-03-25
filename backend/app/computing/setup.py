from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

ext_modules = [
    Pybind11Extension(name="bmi_computing", sources=["src/libexample.cpp"]),
]

setup(
    name="bmi_computing",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)

# python setup.py clean --all
# python setup.py build_ext --build-lib dist
