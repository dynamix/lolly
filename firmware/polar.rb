# generate a polar lookup map
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

# Generate the spiral points
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

# Define the grid size
grid_width = 90
grid_height = 22

# Initialize the lookup array
lookup_array = Array.new(grid_width) { Array.new(grid_height, -1) }

# Calculate the max radius to normalize the coordinates
max_radius = Math.sqrt(511) * 13

# Populate the lookup array
map_2d.each_with_index do |(x, y), index|
  r = Math.sqrt(x**2 + y**2)
  angle = Math.atan2(y, x)
  angle = angle + 2 * Math::PI if angle < 0
  
  # Normalize the radius and angle to fit within the grid
  r_norm = (r / max_radius) * (grid_height - 1)
  angle_norm = (angle / (2 * Math::PI)) * (grid_width - 1)
  
  r_idx = r_norm.round
  angle_idx = angle_norm.round
  
  # Ensure the indices are within the bounds
  r_idx = [r_idx, grid_height - 1].min
  angle_idx = [angle_idx, grid_width - 1].min
  
  # Store the LED index in the lookup array
  lookup_array[angle_idx][r_idx] = index
end

# Print the lookup array
lookup_array.each do |row|
  puts row.map { |v| v.to_s.rjust(3, ' ') }.join(' ')
end

puts lookup_array.transpose.flatten.map { |x| x != -1 ? x : 1000 }.inspect
