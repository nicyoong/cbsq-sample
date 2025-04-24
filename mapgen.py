import random
import math

def generate_map(columns, rows, percent):
    # Define the characters for empty and obstacle
    empty_char = '*'
    obstacle_char = 'O'
    full_char = '#'
    
    # First line: number of lines + characters
    first_line = f"{rows}{empty_char}{obstacle_char}{full_char}"
    
    # Generate the map ensuring only n% obstacles
    total_cells = columns * rows 
    num_obstacles = max(1, math.floor(total_cells * percent / 100))  # n% of total cells, at least 1
    
    # Create a flat list with the required distribution
    map_chars = [obstacle_char] * num_obstacles + [empty_char] * (total_cells - num_obstacles)
    random.shuffle(map_chars)
    
    # Convert into a grid
    map_lines = ["".join(map_chars[i * columns:(i + 1) * columns]) for i in range(rows)]
    
    # Combine the lines with newlines
    final_map = first_line + '\n' + '\n'.join(map_lines) + '\n'
    
    return final_map

filename = "map5.txt"
columns = 20
rows = 20
percent = 4

if __name__ == "__main__":
    for i in range(1, 9):  # Loop from 1 to 8
        filename = f"map{i}.txt"  # Generate filename
        generated_map = generate_map(columns, rows, percent)  # Generate map
        print(f"Saving {filename}...")

        with open(filename, "w") as file:
            file.write(generated_map)
