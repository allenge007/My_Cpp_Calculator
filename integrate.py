# integrate.py
import sys
from sympy import symbols, integrate, latex

def main():
    if len(sys.argv) != 2:
        print("Usage: python integrate.py 'expression'")
        sys.exit(1)

    expr_str = sys.argv[1]
    expr_str = expr_str.replace("^", "**")
    x = symbols('x')
    expr = eval(expr_str)
    result = integrate(expr, x)
    res_str = str(result)
    print(res_str)

if __name__ == "__main__":
    main()