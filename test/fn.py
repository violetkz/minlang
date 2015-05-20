
def fn(n):
    if n == 0 or n == 1:
        return n
    else:
        return fn(n - 1) + fn(n - 2)

print "python fn test"
print fn(30)
