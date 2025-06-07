from setuptools import Extension, setup

module = Extension(
    "cjson",
    sources=["parser.cpp", "cjson.cpp"],
    includes=['cjson.h'],
    extra_compile_args=['-std=c++20']
)

def main():
    setup(
        name="cjson",
        version="1.0",
        description="CJSON module",
        author="D.A. Markhanov",
        ext_modules = [module]
    )

if __name__ == "__main__":
    main()
