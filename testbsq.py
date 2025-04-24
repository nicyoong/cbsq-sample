import subprocess
import math
import statistics

max_hashes = 0
min_hashes = float('inf')
sqrt_hashes = []

for _ in range(100):
    # Run mapgen.py to generate the map
    subprocess.run(["python", "mapgen.py"], check=True)
    
    # Run bsq.py with map5.txt as argument, send an automatic newline to simulate pressing Enter
    # result = subprocess.run(["python", "bsq.py", "map5.txt"], capture_output=True, text=True, check=True)
    result = subprocess.run(["python", "bsq.py", "map1.txt"], capture_output=True, text=True, check=True, input="\n")
    
    # Count occurrences of '#' in the output
    hash_count = result.stdout.count('#')
    
    # Update max_hashes if a new maximum is found
    max_hashes = max(max_hashes, hash_count)
    # Update min_hashes if a new minimum is found
    min_hashes = min(min_hashes, hash_count)
    
    # Store the square root of the hash count
    sqrt_hashes.append(math.sqrt(hash_count))

# Compute square root of the minimum and maximum number of '#' found
sqrt_min_hashes = math.sqrt(min_hashes)
sqrt_max_hashes = math.sqrt(max_hashes)
# Compute median of sqrt_hashes
median_sqrt_hashes = statistics.median(sqrt_hashes)

# Print only the square root values
print("Minimum biggest square dimensions in an iteration:", sqrt_min_hashes)
print("Maximum biggest square dimensions in an iteration:", sqrt_max_hashes)
print("Median of biggest square dimensions:", median_sqrt_hashes)

