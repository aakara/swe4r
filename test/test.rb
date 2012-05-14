require 'test/unit'
require 'swe4r'

class Swe4rTest < Test::Unit::TestCase
  
  def test_swe_set_ephe_path
    assert_equal(nil, Swe4r::swe_set_ephe_path('path'))
  end
  
  def test_swe_julday
    assert_equal(2444838.972916667, Swe4r::swe_julday(1981, 8, 22, 11.35))
  end
  
  def test_swe_set_topo
    assert_equal(nil, Swe4r::swe_set_topo(-112.183333, 45.45, 1524))
  end 
  
  def test_swe_set_sid_mode
    assert_equal(nil, Swe4r::swe_set_sid_mode(Swe4r::SE_SIDM_LAHIRI, 0, 0)) # Use Lahiri mode
    assert_equal(nil, Swe4r::swe_set_sid_mode(Swe4r::SE_SIDM_USER, 2415020.5, 22.460489112721632)) # Use user defined mode
  end
  
  def test_swe_get_ayanamsa_ut
    
    # Test using default sidereal mode
    Swe4r::swe_set_sid_mode(Swe4r::SE_SIDM_FAGAN_BRADLEY, 0, 0)
    assert_equal(24.48395195984284, Swe4r::swe_get_ayanamsa_ut(2444838.972916667))
    
    # Test using Lahari sidereal mode
    Swe4r::swe_set_sid_mode(Swe4r::SE_SIDM_LAHIRI, 0, 0)
    assert_equal(23.600591232207478, Swe4r::swe_get_ayanamsa_ut(2444838.972916667))
    
    # Test using user defined sidereal mode
    Swe4r::swe_set_sid_mode(Swe4r::SE_SIDM_USER, 2415020.5, 22.460489112721632)
    assert_equal(23.60059123097966, Swe4r::swe_get_ayanamsa_ut(2444838.972916667))
    
  end
  
  def test_swe_calc_ut
    
    # The Moshier Ephemeris will be used for all tests since it does not require ephemeris files
    
    # Test #1...
    # Body: Sun
    # Flags: Moshier Ephemeris
    body = Swe4r::swe_calc_ut(2444838.972916667, Swe4r::SE_SUN, Swe4r::SEFLG_MOSEPH)
    assert_equal(149.26565967100066, body[0])
    assert_equal(-0.00012504459645962878, body[1])
    assert_equal(1.0112944920677938, body[2])
    assert_equal(0.0, body[3])
    assert_equal(0.0, body[4])
    assert_equal(0.0, body[5])
    
    # Test #2...
    # Body: Sun
    # Flags: Moshier Ephemeris, High Precision Speed
    body = Swe4r::swe_calc_ut(2444838.972916667, Swe4r::SE_SUN, Swe4r::SEFLG_MOSEPH|Swe4r::SEFLG_SPEED)
    assert_equal(149.26565967100066, body[0])
    assert_equal(-0.00012504459645962907, body[1])
    assert_equal(1.0112944920677933, body[2])
    assert_equal(0.9636052220581458, body[3])
    assert_equal(1.3496040358353975e-05, body[4])
    assert_equal(-0.0002028500243496505, body[5])
    
     # Test #3...
     # Body: Sun
     # Flags: Moshier Ephemeris, High Precision Speed, True Positions
     body = Swe4r::swe_calc_ut(2444838.972916667, Swe4r::SE_SUN, Swe4r::SEFLG_MOSEPH|Swe4r::SEFLG_TRUEPOS|Swe4r::SEFLG_SPEED)
     assert_equal(149.2712876137551, body[0])
     assert_equal(-0.00012494926081564972, body[1])
     assert_equal(1.0112944920677942, body[2])
     assert_equal(0.9636063556074324, body[3])
     assert_equal(1.3496714143588414e-05, body[4])
     assert_equal(-0.00020285002205493142, body[5])
     
     # Test #4...
     # Body: Sun
     # Flags: Moshier Ephemeris, High Precision Speed, True Positions, Topocentric
     Swe4r::swe_set_topo(-112.183333, 45.45, 1524)
     body = Swe4r::swe_calc_ut(2444838.972916667, Swe4r::SE_SUN, Swe4r::SEFLG_MOSEPH|Swe4r::SEFLG_TRUEPOS|Swe4r::SEFLG_SPEED|Swe4r::SEFLG_TOPOCTR)
     assert_equal(149.27327810675615, body[0])
     assert_equal(-0.0013718721637711836, body[1])
     assert_equal(1.0113041739539572, body[2])
     assert_equal(0.9683651963792625, body[3])
     assert_equal(0.0037439499863359716, body[4])
     assert_equal(-0.0003579244367420735, body[5])
    
     # Test #5...
     # Body: Sun
     # Flags: Moshier Ephemeris, High Precision Speed, True Positions, Topocentric, Sidereal (Lahiri Mode)
     Swe4r::swe_set_topo(-112.183333, 45.45, 1524)
     Swe4r::swe_set_sid_mode(1, 0, 0)
     body = Swe4r::swe_calc_ut(2444838.972916667, Swe4r::SE_SUN, Swe4r::SEFLG_MOSEPH|Swe4r::SEFLG_TRUEPOS|Swe4r::SEFLG_SPEED|Swe4r::SEFLG_TOPOCTR|Swe4r::SEFLG_SIDEREAL)
     assert_equal(125.67664442301724, body[0])
     assert_equal(-0.0013718714466436797, body[1])
     assert_equal(1.0113041763508908, body[2])
     assert_equal(0.9683633463076031, body[3])
     assert_equal(0.0037467733857126364, body[4])
     assert_equal(-0.000357918755965452, body[5])
    
  end
  
  def test_swe_houses
    
    # Test each house system
    systems = ['P','K','O','R','C','A','E','V','X','H','T','B','G']
    systems.each do |s|
      houses = Swe4r::swe_houses(2444838.972916667, 45.45, -112.183333, s)
      assert_equal(23, houses.length)
    end
    
    # Test using Placidus house system
    assert_equal([0.0, 133.95429903667724, 153.80292022610058, 178.80796428638726, 210.87357939549094, 248.4391870829004, 284.26552473882526, 313.95429903667724, 333.8029202261006, 358.80796428638723, 30.87357939549094, 68.43918708290038, 104.26552473882526, 133.95429903667724, 30.87357939549094, 28.745752687475147, 273.2404146228894, 116.714052242185, 92.42567828884336, 133.52799465892835, 272.42567828884336, 0.0, 0.0], Swe4r::swe_houses(2444838.972916667, 45.45, -112.183333, 'P'))
    
    
  end
  
end

