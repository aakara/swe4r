#include "ruby.h"
#include "swephexp.h"

// Module
VALUE rb_mSwe4r = Qnil;

// Set directory path of ephemeris files
// int swe_set_ephe_path(char *path);
static VALUE t_swe_set_ephe_path(VALUE self, VALUE path)
{
	swe_set_ephe_path(StringValuePtr(path));
	return Qnil;
}	

// 	Get the Julian day number from year, month, day, hour
// double swe_julday(
// 	int year, 
// 	int month, 
// 	int day, 
// 	double hour,
// 	int gregflag	/* Gregorian calendar: 1, Julian calendar: 0 */
// );
static VALUE t_swe_julday(VALUE self, VALUE year, VALUE month, VALUE day, VALUE hour)
{
	double julday = swe_julday( NUM2INT(year), NUM2INT(month), NUM2INT(day), NUM2DBL(hour), SE_GREG_CAL );
	return rb_float_new(julday);
}

// Set the geographic location for topocentric planet computation
// The longitude and latitude must be in degrees, the altitude in meters.
// void swe_set_topo (
// 	double geolon,      /* geographic longitude: eastern longitude is positive, western longitude is negative */
// 	double geolat,      /* geographic latitude: northern latitude is positive, southern latitude is negative */
// 	double altitude		/* altitude above sea*/
// );	
static VALUE t_swe_set_topo(VALUE self, VALUE lon, VALUE lat, VALUE alt) {
	swe_set_topo(NUM2DBL(lon),NUM2DBL(lat),NUM2DBL(alt));
	return Qnil;
}

// Calculation of planets, moon, asteroids, lunar nodes, apogees, fictitious bodies
// long swe_calc_ut(
// 	double tjd_ut,	/* Julian day number, Universal Time */
// 	int ipl,		/* planet number */
// 	long iflag,		/* flag bits */
// 	double *xx,  	/* target address for 6 position values: longitude, latitude, distance, long.speed, lat.speed, dist. peed */
// 	char *serr		/* 256 bytes for error string */
// );
static VALUE t_swe_calc_ut(VALUE self, VALUE julian_ut, VALUE body, VALUE iflag) {
	double results[6];
	char serr[AS_MAXCH];
	VALUE arr = rb_ary_new();
	int id_push = rb_intern("push");
	int i=0;
	
	if ( swe_calc_ut(NUM2DBL(julian_ut), NUM2INT(body), NUM2LONG(iflag), results,  serr) < 0 )
		rb_raise (rb_eRuntimeError, serr);
	
	for ( i = 0; i < 6; i++)
		rb_funcall(arr, id_push, 1, rb_float_new(results[i]));
	
	return arr;
}

// This function can be used to specify the mode for sidereal computations.
// void swe_set_sid_mode (
// int32 sid_mode, 	/* Mode */
// double t0, 		/* Reference date */
// double ayan_t0	/* Initial value of the ayanamsha */
// );
static VALUE t_swe_set_sid_mode(VALUE self, VALUE mode, VALUE t0, VALUE ayan_t0) {
	swe_set_sid_mode(NUM2INT(mode), NUM2DBL(t0), NUM2DBL(ayan_t0));
	return Qnil;
}

// This function computes the ayanamsha, the distance of the tropical vernal point from the sidereal zero point of the zodiac. 
// The ayanamsha is used to compute sidereal planetary positions from tropical ones:
// pos_sid = pos_trop – ayanamsha
// Before calling swe_get_ayanamsha(), you have to set the sidereal mode with swe_set_sid_mode, unless you want the default sidereal mode, which is the Fagan/Bradley ayanamsha.
// double swe_get_ayanamsa_ut(double tjd_ut);
static VALUE t_swe_get_ayanamsa_ut(VALUE self, VALUE julian_ut) {
	double ayanamsa = swe_get_ayanamsa_ut(NUM2DBL(julian_ut));
	return rb_float_new(ayanamsa);
}

// This function computes house cusps, ascendant, midheaven, etc
// int swe_houses(
// double tjd_ut,      /* Julian day number, UT */
// double geolat,      /* geographic latitude, in degrees */
// double geolon,      /* geographic longitude, in degrees
// 						* eastern longitude is positive,
//						* western longitude is negative,
// 						* northern latitude is positive,
// 						* southern latitude is negative */	
// int hsys,           /* house method, ascii code of one of the letters PKORCAEVXHTBG */
// double *cusps,      /* array for 13 doubles */
// double *ascmc);     /* array for 10 doubles */
static VALUE t_swe_houses(VALUE self, VALUE julian_day, VALUE latitude, VALUE longitude, VALUE house_system)
{
	double cusps[13];
	double ascmc[10];
	char serr[AS_MAXCH];
	VALUE arr = rb_ary_new();
	int id_push = rb_intern("push");
	int i =0;
	
	if ( swe_houses(NUM2DBL(julian_day), NUM2DBL(latitude), NUM2DBL(longitude), NUM2CHR(house_system), cusps, ascmc) < 0 )
		rb_raise (rb_eRuntimeError, serr);
	
	for ( i = 0; i < 13; i++)
		rb_funcall(arr, id_push, 1, rb_float_new(cusps[i]));
	
	for ( i = 0; i < 10; i++)
		rb_funcall(arr, id_push, 1, rb_float_new(ascmc[i]));
	
	return arr;
}

void Init_swe4r()
{ 
	rb_mSwe4r = rb_define_module ("Swe4r");
	rb_define_method(rb_mSwe4r, "swe_set_ephe_path", t_swe_set_ephe_path, 1);
	rb_define_method(rb_mSwe4r, "swe_julday", t_swe_julday, 4);
	rb_define_method(rb_mSwe4r, "swe_set_topo", t_swe_set_topo, 3);
	rb_define_method(rb_mSwe4r, "swe_calc_ut", t_swe_calc_ut, 3);
	rb_define_method(rb_mSwe4r, "swe_set_sid_mode", t_swe_set_sid_mode, 3);
	rb_define_method(rb_mSwe4r, "swe_get_ayanamsa_ut", t_swe_get_ayanamsa_ut, 1);
	rb_define_method(rb_mSwe4r, "swe_houses", t_swe_houses, 4);
}