import subprocess

# List of map files
map_files = [f"map{i}.txt" for i in range(1, 9)]

# Function to run bsq.py with given arguments
def run_bsq(*args):
    command = ["python", "bsq.py"] + list(args)
    print(f"Running: {' '.join(command)}")
    # subprocess.run(command)
    subprocess.run(command, input="\n", text=True)

if __name__ == "__main__":
    # Test case 1: Run with a single argument
    run_bsq(map_files[0])

    # Test case 2: Run with two arguments
    run_bsq(map_files[0], map_files[1])

    # Test case 3: Run with all eight arguments
    run_bsq(*map_files)
