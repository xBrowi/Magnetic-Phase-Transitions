import time 
import random

timestart = time.time()
    
sum = 0
for i in range(100_000_000):
    sum += random.randint(1, 10000)

timeend = time.time()
print("Summen er:", sum)
print("Tid brukt:", timeend - timestart, "sekunder")