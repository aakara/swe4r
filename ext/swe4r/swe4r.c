/*
Swe4r :: Swiss Ephemeris for Ruby - A C extension for the Swiss Ephemeris library (http://www.astro.com/swisseph/)
Copyright (C) 2012 Andrew Kirk (andrew.kirk@windhorsemedia.com)

This file is part of Swe4r.

Swe4r is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Swe4r is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Swe4r.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "ruby.h"
#include "swephexp.h"

// Module Name
VALUE rb_mSwe4r = Qnil;

/*
 * Body numbers used by the swe_calc_ut() method
 * See http://www.astro.com/swisseph/swephprg.htm#_Toc283735422
 */
#define SE_SUN          0       /* Sun */
#define SE_MOON         1       /* Moon */
#define SE_MERCURY      2       /* Mercury */
#define SE_VENUS        3       /* Venus */
#define SE_MARS         4       /* Mars */
#define SE_JUPITER      5       /* Jupiter */
#define SE_SATURN       6       /* Saturn */
#define SE_URANUS       7       /* Uranus */
#define SE_NEPTUNE      8       /* Neptune */
#define SE_PLUTO        9       /* Pluto */
#define SE_MEAN_NODE    10      /* Mean North/Ascending Node */
#define SE_TRUE_NODE    11		/* True North/Ascending Node */
#define SE_MEAN_APOG    12      /* Mean Apogee */
#define SE_OSCU_APOG    13    	/* Osculating Apogee */
#define SE_EARTH        14      /* Earth */
#define SE_CHIRON       15      /* Chiron */
#define SE_PHOLUS       16      /* Pholus */
#define SE_CERES        17      /* Ceres */
#define SE_PALLAS       18      /* Pallas */
#define SE_JUNO         19      /* Juno */
#define SE_VESTA        20      /* Vesta */
#define SE_INTP_APOG    21      /* Interpolated Apogee */
#define SE_INTP_PERG    22		/* Interpolated Perigee */

/*
 * Flag bits used by the swe_calc_ut() method
 * See http://www.astro.com/swisseph/swephprg.htm#_Toc283735427
 */
#define SEFLG_JPLEPH    	1       	/* use JPL ephemeris */
#define SEFLG_SWIEPH    	2       	/* use SWISSEPH ephemeris */
#define SEFLG_MOSEPH    	4       	/* use Moshier ephemeris */
#define SEFLG_HELCTR		8      		/* return heliocentric position */
#define SEFLG_TRUEPOS		16     		/* return true positions, not apparent */
#define SEFLG_J2000			32     		/* no precession, i.e. give J2000 equinox */
#define SEFLG_NONUT			64     		/* no nutation, i.e. mean equinox of date */
#define SEFLG_SPEED3		128     	/* speed from 3 positions (do not use it, SEFLG_SPEED is faster and more precise.) */
#define SEFLG_SPEED			256     	/* high precision speed  */
#define SEFLG_NOGDEFL		512     	/* turn off gravitational deflection */
#define SEFLG_NOABERR		1024    	/* turn off 'annual' aberration of light */
#define SEFLG_EQUATORIAL	(2*1024)   	/* equatorial positions are wanted */
#define SEFLG_XYZ			(4*1024)    /* cartesian, not polar, coordinates */
#define SEFLG_RADIANS		(8*1024)    /* coordinates in radians, not degrees */
#define SEFLG_BARYCTR		(16*1024)   /* barycentric positions */
#define SEFLG_TOPOCTR		(32*1024)   /* topocentric positions */
#define SEFLG_SIDEREAL		(64*1024)   /* sidereal positions */
#define SEFLG_ICRS			(128*1024)  /* ICRS (DE406 reference frame) */

/*
 * Sidereal modes used by the swe_set_sid_mode() method
 * See http://www.astro.com/swisseph/swephprg.htm#_Toc283735477
 */
#define SE_SIDM_FAGAN_BRADLEY    0		/* Fagan/Bradley) */
#define SE_SIDM_LAHIRI           1		/* Lahiri */
#define SE_SIDM_DELUCE           2		/* De Luce */
#define SE_SIDM_RAMAN            3		/* Raman */
#define SE_SIDM_USHASHASHI       4		/* Ushashashi */
#define SE_SIDM_KRISHNAMURTI     5		/* Krishnamurti */
#define SE_SIDM_DJWHAL_KHUL      6		/* Djwhal Khul */
#define SE_SIDM_YUKTESHWAR       7		/* Yukteshvar */
#define SE_SIDM_JN_BHASIN        8		/* JN Bhasin */
#define SE_SIDM_BABYL_KUGLER1    9		/* Babylonian Kugler 1 */
#define SE_SIDM_BABYL_KUGLER2   10		/* Babylonian Kugler 2 */
#define SE_SIDM_BABYL_KUGLER3   11		/* Babylonian Kugler 3 */
#define SE_SIDM_BABYL_HUBER    	12		/* Babylonian Huber*/
#define SE_SIDM_BABYL_ETPSC    	13		/* Babylonian Spica at 29 vi 21 */
#define SE_SIDM_ALDEBARAN_15TAU 14		/* Aldebaran 15 Taurus */
#define SE_SIDM_HIPPARCHOS      15		/* Hipparchos */
#define SE_SIDM_SASSANIAN       16		/* Sassanian */
#define SE_SIDM_GALCENT_0SAG    17		/* Galactic Center at 0 Sagittarius */
#define SE_SIDM_J2000           18		/* J2000 */
#define SE_SIDM_J1900           19		/* J1900 */
#define SE_SIDM_B1950           20		/* B1950 */
#define SE_SIDM_USER            255		/* User Defined */

/*
 * Set directory path of ephemeris files
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735481
 * int swe_set_ephe_path(char *path);
 */
static VALUE t_swe_set_ephe_path(VALUE self, VALUE path)
{
	swe_set_ephe_path(StringValuePtr(path));
	return Qnil;
}	

/*
 * Get the Julian day number from year, month, day, hour
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735468
	double swe_julday(
 		int year, 
		int month, 
		int day, 
		double hour,
		int gregflag	// Gregorian calendar: 1, Julian calendar: 0 
	);
 */
static VALUE t_swe_julday(VALUE self, VALUE year, VALUE month, VALUE day, VALUE hour)
{
	double julday = swe_julday( NUM2INT(year), NUM2INT(month), NUM2INT(day), NUM2DBL(hour), SE_GREG_CAL );
	return rb_float_new(julday);
}

/*
 * Set the geographic location for topocentric planet computation
 * The longitude and latitude must be in degrees, the altitude in meters.
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735476
	void swe_set_topo (
		double geolon,      // geographic longitude: eastern longitude is positive, western longitude is negative
		double geolat,      // geographic latitude: northern latitude is positive, southern latitude is negative
		double altitude		// altitude above sea
	);
*/
static VALUE t_swe_set_topo(VALUE self, VALUE lon, VALUE lat, VALUE alt) {
	swe_set_topo(NUM2DBL(lon),NUM2DBL(lat),NUM2DBL(alt));
	return Qnil;
}

/*
 * Calculation of planets, moon, asteroids, lunar nodes, apogees, fictitious bodies
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735419
	long swe_calc_ut(
		double tjd_ut,	// Julian day number, Universal Time
		int ipl,		// planet number
		long iflag,		// flag bits
		double *xx,  	// target address for 6 position values: longitude, latitude, distance, long.speed, lat.speed, dist.speed
		char *serr		// 256 bytes for error string
	);
 */
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

/*
 * This function can be used to specify the mode for sidereal computations
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735478
	void swe_set_sid_mode (
		int32 sid_mode, 	// Mode
		double t0, 		// Reference date
		double ayan_t0	// Initial value of the ayanamsha
	);
 */
static VALUE t_swe_set_sid_mode(VALUE self, VALUE mode, VALUE t0, VALUE ayan_t0) {
	swe_set_sid_mode(NUM2INT(mode), NUM2DBL(t0), NUM2DBL(ayan_t0));
	return Qnil;
}

/*
 * This function computes the ayanamsha, the distance of the tropical vernal point from the sidereal zero point of the zodiac.
 * The ayanamsha is used to compute sidereal planetary positions from tropical ones:
 * pos_sid = pos_trop – ayanamsha
 * Before calling swe_get_ayanamsha(), you have to set the sidereal mode with swe_set_sid_mode, unless you want the default sidereal mode, which is the Fagan/Bradley ayanamsha.
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735479
 * double swe_get_ayanamsa_ut(double tjd_ut);
 */
static VALUE t_swe_get_ayanamsa_ut(VALUE self, VALUE julian_ut) {
	double ayanamsa = swe_get_ayanamsa_ut(NUM2DBL(julian_ut));
	return rb_float_new(ayanamsa);
}

/*
 * This function computes house cusps, ascendant, midheaven, etc
 * http://www.astro.com/swisseph/swephprg.htm#_Toc283735486
	int swe_houses(
		double tjd_ut,      // Julian day number, UT
		double geolat,      // geographic latitude, in degrees
		double geolon,      // geographic longitude, in degrees (eastern longitude is positive, western longitude is negative, northern latitude is positive, southern latitude is negative	
		int hsys,           // house method, ascii code of one of the letters PKORCAEVXHTBG
		double *cusps,      // array for 13 doubles
		double *ascmc	    // array for 10 doubles
	);
 * House method codes...
 * ‘P’ 			= Placidus
 * ‘K’     		= Koch
 * ‘O’     		= Porphyrius
 * ‘R’     		= Regiomontanus
 * ‘C’     		= Campanus
 * ‘A’ or ‘E’  	= Equal (cusp 1 is Ascendant)
 * ‘V’     		= Vehlow equal (Asc. in middle of house 1)
 * ‘W’     		= Whole sign
 * ‘X’     		= axial rotation system
 * ‘H’     		= azimuthal or horizontal system
 * ‘T’     		= Polich/Page (“topocentric” system)
 * ‘B’     		= Alcabitus
 * ‘M’     		= Morinus
 * ‘U’     		= Krusinski-Pisa
 * ‘G’     		= Gauquelin sectors
 */
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
	// Module
	rb_mSwe4r = rb_define_module ("Swe4r");
	
	// Module Functions
	rb_define_module_function(rb_mSwe4r, "swe_set_ephe_path", t_swe_set_ephe_path, 1);
	rb_define_module_function(rb_mSwe4r, "swe_julday", t_swe_julday, 4);
	rb_define_module_function(rb_mSwe4r, "swe_set_topo", t_swe_set_topo, 3);
	rb_define_module_function(rb_mSwe4r, "swe_calc_ut", t_swe_calc_ut, 3);
	rb_define_module_function(rb_mSwe4r, "swe_set_sid_mode", t_swe_set_sid_mode, 3);
	rb_define_module_function(rb_mSwe4r, "swe_get_ayanamsa_ut", t_swe_get_ayanamsa_ut, 1);
	rb_define_module_function(rb_mSwe4r, "swe_houses", t_swe_houses, 4);
	
	// Constants
	rb_define_const(rb_mSwe4r, "SE_SUN", INT2FIX(SE_SUN));
	rb_define_const(rb_mSwe4r, "SE_MOON", INT2FIX(SE_MOON));
	rb_define_const(rb_mSwe4r, "SE_MERCURY", INT2FIX(SE_MERCURY));
	rb_define_const(rb_mSwe4r, "SE_VENUS", INT2FIX(SE_VENUS));
	rb_define_const(rb_mSwe4r, "SE_MARS", INT2FIX(SE_MARS));
	rb_define_const(rb_mSwe4r, "SE_JUPITER", INT2FIX(SE_JUPITER));
	rb_define_const(rb_mSwe4r, "SE_SATURN", INT2FIX(SE_SATURN));
	rb_define_const(rb_mSwe4r, "SE_URANUS", INT2FIX(SE_URANUS));
	rb_define_const(rb_mSwe4r, "SE_NEPTUNE", INT2FIX(SE_NEPTUNE));
	rb_define_const(rb_mSwe4r, "SE_PLUTO", INT2FIX(SE_PLUTO));
	rb_define_const(rb_mSwe4r, "SE_MEAN_NODE", INT2FIX(SE_MEAN_NODE));
	rb_define_const(rb_mSwe4r, "SE_TRUE_NODE", INT2FIX(SE_TRUE_NODE));
	rb_define_const(rb_mSwe4r, "SE_MEAN_APOG", INT2FIX(SE_MEAN_APOG));
	rb_define_const(rb_mSwe4r, "SE_OSCU_APOG", INT2FIX(SE_OSCU_APOG));
	rb_define_const(rb_mSwe4r, "SE_EARTH", INT2FIX(SE_EARTH));
	rb_define_const(rb_mSwe4r, "SE_CHIRON", INT2FIX(SE_CHIRON));
	rb_define_const(rb_mSwe4r, "SE_PHOLUS", INT2FIX(SE_PHOLUS));
	rb_define_const(rb_mSwe4r, "SE_CERES", INT2FIX(SE_CERES));
	rb_define_const(rb_mSwe4r, "SE_PALLAS", INT2FIX(SE_PALLAS));
	rb_define_const(rb_mSwe4r, "SE_JUNO", INT2FIX(SE_JUNO));
	rb_define_const(rb_mSwe4r, "SE_VESTA", INT2FIX(SE_VESTA));
	rb_define_const(rb_mSwe4r, "SE_INTP_APOG", INT2FIX(SE_INTP_APOG));
	rb_define_const(rb_mSwe4r, "SE_INTP_PERG", INT2FIX(SE_INTP_PERG));
	
	rb_define_const(rb_mSwe4r, "SEFLG_JPLEPH", INT2FIX(SEFLG_JPLEPH));
	rb_define_const(rb_mSwe4r, "SEFLG_SWIEPH", INT2FIX(SEFLG_SWIEPH));
	rb_define_const(rb_mSwe4r, "SEFLG_MOSEPH", INT2FIX(SEFLG_MOSEPH));
	rb_define_const(rb_mSwe4r, "SEFLG_HELCTR", INT2FIX(SEFLG_HELCTR));
	rb_define_const(rb_mSwe4r, "SEFLG_TRUEPOS", INT2FIX(SEFLG_TRUEPOS));
	rb_define_const(rb_mSwe4r, "SEFLG_J2000", INT2FIX(SEFLG_J2000));
	rb_define_const(rb_mSwe4r, "SEFLG_NONUT", INT2FIX(SEFLG_NONUT));
	rb_define_const(rb_mSwe4r, "SEFLG_SPEED3", INT2FIX(SEFLG_SPEED3));
	rb_define_const(rb_mSwe4r, "SEFLG_SPEED", INT2FIX(SEFLG_SPEED));
	rb_define_const(rb_mSwe4r, "SEFLG_NOGDEFL", INT2FIX(SEFLG_NOGDEFL));
	rb_define_const(rb_mSwe4r, "SEFLG_NOABERR", INT2FIX(SEFLG_NOABERR));
	rb_define_const(rb_mSwe4r, "SEFLG_EQUATORIAL", INT2FIX(SEFLG_EQUATORIAL));
	rb_define_const(rb_mSwe4r, "SEFLG_XYZ", INT2FIX(SEFLG_XYZ));
	rb_define_const(rb_mSwe4r, "SEFLG_RADIANS", INT2FIX(SEFLG_RADIANS));
	rb_define_const(rb_mSwe4r, "SEFLG_BARYCTR", INT2FIX(SEFLG_BARYCTR));
	rb_define_const(rb_mSwe4r, "SEFLG_TOPOCTR", INT2FIX(SEFLG_TOPOCTR));
	rb_define_const(rb_mSwe4r, "SEFLG_SIDEREAL", INT2FIX(SEFLG_SIDEREAL));
	rb_define_const(rb_mSwe4r, "SEFLG_ICRS", INT2FIX(SEFLG_ICRS));
	
	rb_define_const(rb_mSwe4r, "SE_SIDM_FAGAN_BRADLEY", INT2FIX(SE_SIDM_FAGAN_BRADLEY));
	rb_define_const(rb_mSwe4r, "SE_SIDM_LAHIRI", INT2FIX(SE_SIDM_LAHIRI));
	rb_define_const(rb_mSwe4r, "SE_SIDM_DELUCE", INT2FIX(SE_SIDM_DELUCE));
	rb_define_const(rb_mSwe4r, "SE_SIDM_RAMAN", INT2FIX(SE_SIDM_RAMAN));
	rb_define_const(rb_mSwe4r, "SE_SIDM_USHASHASHI", INT2FIX(SE_SIDM_USHASHASHI));
	rb_define_const(rb_mSwe4r, "SE_SIDM_KRISHNAMURTI", INT2FIX(SE_SIDM_KRISHNAMURTI));
	rb_define_const(rb_mSwe4r, "SE_SIDM_DJWHAL_KHUL", INT2FIX(SE_SIDM_DJWHAL_KHUL));
	rb_define_const(rb_mSwe4r, "SE_SIDM_YUKTESHWAR", INT2FIX(SE_SIDM_YUKTESHWAR));
	rb_define_const(rb_mSwe4r, "SE_SIDM_JN_BHASIN", INT2FIX(SE_SIDM_JN_BHASIN));
	rb_define_const(rb_mSwe4r, "SE_SIDM_BABYL_KUGLER1", INT2FIX(SE_SIDM_BABYL_KUGLER1));
	rb_define_const(rb_mSwe4r, "SE_SIDM_BABYL_KUGLER2", INT2FIX(SE_SIDM_BABYL_KUGLER2));
	rb_define_const(rb_mSwe4r, "SE_SIDM_BABYL_KUGLER3", INT2FIX(SE_SIDM_BABYL_KUGLER3));
	rb_define_const(rb_mSwe4r, "SE_SIDM_BABYL_HUBER", INT2FIX(SE_SIDM_BABYL_HUBER));
	rb_define_const(rb_mSwe4r, "SE_SIDM_BABYL_ETPSC", INT2FIX(SE_SIDM_BABYL_ETPSC));
	rb_define_const(rb_mSwe4r, "SE_SIDM_ALDEBARAN_15TAU", INT2FIX(SE_SIDM_ALDEBARAN_15TAU));
	rb_define_const(rb_mSwe4r, "SE_SIDM_HIPPARCHOS", INT2FIX(SE_SIDM_HIPPARCHOS));
	rb_define_const(rb_mSwe4r, "SE_SIDM_SASSANIAN", INT2FIX(SE_SIDM_SASSANIAN));
	rb_define_const(rb_mSwe4r, "SE_SIDM_GALCENT_0SAG", INT2FIX(SE_SIDM_GALCENT_0SAG));
	rb_define_const(rb_mSwe4r, "SE_SIDM_J2000", INT2FIX(SE_SIDM_J2000));
	rb_define_const(rb_mSwe4r, "SE_SIDM_J1900", INT2FIX(SE_SIDM_J1900));
	rb_define_const(rb_mSwe4r, "SE_SIDM_B1950", INT2FIX(SE_SIDM_B1950));
	rb_define_const(rb_mSwe4r, "SE_SIDM_USER", INT2FIX(SE_SIDM_USER));
}