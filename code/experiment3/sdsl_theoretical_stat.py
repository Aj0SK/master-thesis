from math import comb, pow, log, ceil

def expected_c_i(n, b, p, i):
    return (n / b) * comb(b, i) * pow(p, i) * pow(1 - p, b - i)

def theoretical(n, b, cutoff, p):
    result_classic = (n / b) * ceil(log(b + 1, 2))
    result_hybrid = (n / b) * ceil(log(cutoff + 1, 2))
    for i in range(0, b+1):
        result_classic += expected_c_i(n, b, p, i) * log(comb(b, i), 2)
    for i in range(0, cutoff):
        result_hybrid += expected_c_i(n, b, p, i) * log(comb(b, i), 2)
    for i in range(cutoff, b+1):
        result_hybrid += expected_c_i(n, b, p, i) * b
    rate = result_hybrid/result_classic
    print("For", n, b, cutoff, p, "the result is", result_classic, "vs", result_hybrid, "=", rate)

for b in [31, 63, 127]:
    for i in [7, 15, 31, 63]:
        if i < b:
            theoretical(b*1000, b, i, 0.05)