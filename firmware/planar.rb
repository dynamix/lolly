
# generate a planar lookup map

GRID_SIZE_X = 35
GRID_SIZE_Y = 35

arms = []
[1, 9, 17, 4, 12, 20, 7, 15, 2, 10, 18, 5, 13, 21, 8, 16, 3, 11, 19, 6, 14].each_with_index do |j, stripidx|
  strip = []
  i = j
  while i <= 512
    strip << i
    i += 21
  end
  strip.reverse! if stripidx.odd?
  arms.concat(strip)
end


map_2d = []
(0...512).each do |n|
  n = arms[n]
  r = Math.sqrt(n) * 13
  deg = n * 137.508
  angle = (deg / 180.0) * Math::PI
  x = r * Math.cos(angle)
  y = r * Math.sin(angle)
  map_2d << [x, y]
end

# Step 2: Normalize positions to fit within a 40x40 grid
# Find min and max for x and y
min_x, max_x = map_2d.map { |pos| pos[0] }.minmax
min_y, max_y = map_2d.map { |pos| pos[1] }.minmax

# Define a function to normalize a value to a grid size
def normalize(value, min, max, grid_size)
  ((value - min) / (max - min) * (grid_size - 1)).round
end

# Normalize all positions
normalized_positions = map_2d.map do |(x, y)|
  norm_x = normalize(x, min_x, max_x, GRID_SIZE_X)
  norm_y = normalize(y, min_y, max_y, GRID_SIZE_Y)
  [norm_x, norm_y]
end

# Step 3: Create the 40x40 lookup array and fill it
lookup_array = Array.new( GRID_SIZE_X ) { Array.new( GRID_SIZE_Y, nil) }

# Define the center and radius of the circle
center_x, center_y = GRID_SIZE_X / 2, GRID_SIZE_Y / 2
radius = GRID_SIZE_X / 2

# Fill the lookup array with the nearest LED index if within the radius
(0... GRID_SIZE_X ).each do |i|
  (0...GRID_SIZE_Y).each do |j|
    if Math.sqrt((i - center_x) ** 2 + (j - center_y) ** 2) <= radius
      min_distance = Float::INFINITY
      nearest_index = nil
      normalized_positions.each_with_index do |(nx, ny), index|
        distance = Math.sqrt((nx - i) ** 2 + (ny - j) ** 2)
        if distance < min_distance
          min_distance = distance
          nearest_index = index
        end
      end
      lookup_array[i][j] = nearest_index
    end
  end
end

# Print the lookup array
lookup_array.each do |row|
  puts row.map { |index| index.nil? ? '.  ' : index.to_s.rjust(3) }.join(' ')
end

puts lookup_array.flatten.map { |x| x ? x : 1000 }.inspect


