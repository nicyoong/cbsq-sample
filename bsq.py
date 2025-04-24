import sys

def parse_map(file_content):
    lines = file_content.splitlines()
    if len(lines) < 2:
        return None, "map error"
    
    # First line parsing
    first_line = lines[0]
    if len(first_line) < 4:
        return None, "map error"
    
    try:
        num_lines = int(first_line[:-3])
        empty_char, obstacle_char, full_char = first_line[-3:]
    except ValueError:
        return None, "map error"
    
    # Validating uniqueness of characters
    if len(set([empty_char, obstacle_char, full_char])) != 3:
        return None, "map error"
    
    # Map parsing and validation
    map_data = lines[1:]
    if len(map_data) != num_lines:
        return None, "map error"
    
    map_width = len(map_data[0])
    if map_width == 0:
        return None, "map error"
    
    for row in map_data:
        if len(row) != map_width or any(c not in (empty_char, obstacle_char) for c in row):
            return None, "map error"
    
    return (num_lines, map_width, empty_char, obstacle_char, full_char, map_data), None

def find_largest_square(map_info):
    num_lines, map_width, empty_char, obstacle_char, full_char, map_data = map_info
    
    dp = [[0] * map_width for _ in range(num_lines)]
    max_size = 0
    bottom_right = (0, 0)
    
    for i in range(num_lines):
        for j in range(map_width):
            if map_data[i][j] == empty_char:
                if i == 0 or j == 0:
                    dp[i][j] = 1
                else:
                    dp[i][j] = min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1
                
                if dp[i][j] > max_size:
                    max_size = dp[i][j]
                    bottom_right = (i, j)
    
    # Mark the largest square
    bi, bj = bottom_right
    for i in range(bi - max_size + 1, bi + 1):
        map_data[i] = (map_data[i][:bj - max_size + 1] + 
                        full_char * max_size + 
                        map_data[i][bj + 1:])
    
    return "\n".join(map_data)

def process_file(filename):
    try:
        with open(filename, 'r') as f:
            file_content = f.read()
        
        map_info, error = parse_map(file_content)
        if error:
            sys.stderr.write("map error\n")
        else:
            print(find_largest_square(map_info))
    except Exception:
        sys.stderr.write("map error\n")

def main():
    if len(sys.argv) > 1:
        for filename in sys.argv[1:]:
            process_file(filename)
            print()  # Line break after each file's output
    else:
        # Read from standard input
        file_content = sys.stdin.read()
        map_info, error = parse_map(file_content)
        if error:
            sys.stderr.write("map error\n")
        else:
            print(find_largest_square(map_info))

if __name__ == "__main__":
    main()

input()
