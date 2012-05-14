require 'swe4r'

#############################
# CONFIGURATION
#############################

# Date Information
year = 2012
month = 5
day = 14
hour = 10.15

# Geographic Location
longitude = -112.183333
latitidue = 45.45
altitude = 1468

#############################
# MAIN
#############################

# Get the Julian day number
jd = Swe4r::swe_julday(year, month, day, hour)

# Get house details using the Placidus house system
houses = Swe4r::swe_houses(jd, latitidue, longitude, 'P')

# Print the house cusps
(1..12).each do |i|
  puts "House ##{i} Cusp: #{houses[i]}"
end

# Print ascendant, midheaven, etc
puts "Ascendant: #{houses[13]}"
puts "Midheaven (MC): #{houses[14]}"
puts "ARMC: #{houses[15]}"
puts "Equatorial Ascendant: #{houses[16]}"
puts "Co-Ascendant (Walter Koch): #{houses[17]}"
puts "Co-Ascendant (Michael Munkasey): #{houses[18]}"
puts "Polar Ascendant (M. Munkasey) : #{houses[19]}"
