Gem::Specification.new do |s|
  s.name              = 'swe4r'
  s.version           = '0.0.1'
  s.date              = '2012-05-13'
  s.summary           = "Swiss Ephemeris for Ruby"
  s.description       = "A C extension for the Swiss Ephemeris library (http://www.astro.com/swisseph/)"
  s.author            = "Andrew Kirk"
  s.email             = "andrew.kirk@windhorsemedia.com"
  s.license           = "GPL-2"
  s.extra_rdoc_files  = ['README.rdoc']
  s.files             = Dir.glob('lib/**/*.{rb}') + Dir.glob('ext/**/*.{rb,h,c}')
  s.extensions        = ['ext/swe4r/extconf.rb']
end