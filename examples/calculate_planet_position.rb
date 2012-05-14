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

# Set the geographic location for topocentric positions
Swe4r::swe_set_topo(longitude, latitidue, altitude)

# Set the sidereal mode for sidereal positions
Swe4r::swe_set_sid_mode(Swe4r::SE_SIDM_LAHIRI, 0, 0)

# Get the ayanamsha (the distance of the tropical vernal point from the sidereal zero point of the zodiac)
ayanamsha = Swe4r::swe_get_ayanamsa_ut(jd)

# Calculate the position of the Sun
# Use the Moshier Ephemeris (does not require ephemeris files)
# Get high precision speed and sidereal/topocentric positions
body = Swe4r::swe_calc_ut(jd, Swe4r::SE_SUN, Swe4r::SEFLG_MOSEPH|Swe4r::SEFLG_SPEED|Swe4r::SEFLG_TOPOCTR|Swe4r::SEFLG_SIDEREAL)

# Print the results
puts "Longitude: #{body[0]}"
puts "Latitude: #{body[1]}"
puts "Distance in AU: #{body[2]}"
puts "Speed in longitude (deg/day): #{body[3]}"
puts "Speed in latitude (deg/day): #{body[4]}"
puts "Speed in distance (AU/day): #{body[5]}"
puts "Ayanamsha: #{ayanamsha}"

