/*----------------------------------------------------------------------------
 * Name:    Planetary_Positioning.c
 * Purpose: Planetary Position Computation
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		01/02/2018
 *----------------------------------------------------------------------------*/

#include "Planetary_Positioning.h"
#include "Math.h"

#define PI          3.14159265358979323846
#define RADEG       (180.0/PI)
#define DEGRAD      (PI/180.0)
#define sind(x)     sin((x)*DEGRAD)
#define cosd(x)     cos((x)*DEGRAD)
#define tand(x)     tan((x)*DEGRAD)
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atand(x)    (RADEG*atan(x))
#define atan2d(y,x) (RADEG*atan2((y),(x)))

ORBIT Sun,Mercury,Venus,Earth,Mars,Jupiter,Saturn,Uranus,Neptune;

double d;	// Global variables storing the reference date calculated by ref_date();

// Main Planet Function
void PosComp(int year,int month, int date, double time){
	ref_date(year,month,date,time);
	compute_orbital_elements();
	sunPos();
	planetsPos(&Mercury);
	planetsPos(&Venus);
	planetsPos(&Earth);
	planetsPos(&Mars);
	planetsPos(&Jupiter);
	planetsPos(&Saturn);
	planetsPos(&Uranus);
	planetsPos(&Neptune);
	perturbations();
}


// Rev used to normalise double precision number to 0 < x < 360
double rev( double x ){
	return  x - floor(x/360.0)*360.0;
}

// Computes reference date number from input Julian Date
// Time is in hours with minutes and second being decimal places
double ref_date(int year,int month,int date,double time){
	double d1 = 367*year - 7 * ( year + (month+9)/12 ) / 4 + 275*month/9 + date - 730530;
	d1 = d1 + time/24.0;
	d=d1;
	return d1;
}

// Computes initial orbital elements of all planets based on ref date d
void compute_orbital_elements(){

	// Sun/Earth
	Sun.N = 0.0;
	Sun.i = 0.0;
	Sun.w = rev(282.9404 + 4.70935E-5 * d);
	Sun.a = 1.0;
	Sun.e = 0.016709 - 1.151E-9 * d;
	Sun.M = rev(356.0470 + 0.9856002585 * d);

	// Mercury
	Mercury.N = rev(48.3313 + 3.24587E-5 * d);
	Mercury.i = rev(7.0047 + 5.00E-8 * d);
	Mercury.w = rev(29.1241 + 1.01444E-5 * d);
	Mercury.a = 0.387098;
	Mercury.e = 0.205635 + 5.59E-10 * d;
	Mercury.M = rev(168.6562 + 4.0923344368 * d);

	// Venus
	Venus.N = rev(76.6799 + 2.46590E-5 * d);
	Venus.i = rev(3.3946 + 2.75E-8 * d);
	Venus.w = rev(54.8910 + 1.38374E-5 * d);
	Venus.a = 0.723330;
	Venus.e = 0.006773 - 1.302E-9 * d;
	Venus.M = rev(48.0052 + 1.6021302244 * d);

	// Mars
	Mars.N = rev(49.5574 + 2.11081E-5 * d);
  Mars.i = rev(1.8497 - 1.78E-8 * d);
  Mars.w = rev(286.5016 + 2.92961E-5 * d);
  Mars.a = 1.523688;
  Mars.e = 0.093405 + 2.516E-9 * d;
  Mars.M = rev(18.6021 + 0.5240207766 * d);

	// Jupiter
	Jupiter.N = rev(100.4542 + 2.76854E-5 * d);
	Jupiter.i = rev(1.3030 - 1.557E-7 * d);
	Jupiter.w = rev(273.8777 + 1.64505E-5 * d);
	Jupiter.a = 5.20256;
	Jupiter.e = 0.048498 + 4.469E-9 * d;
	Jupiter.M = rev(19.8950 + 0.0830853001 * d);

	// Saturn
	Saturn.N = rev(113.6634 + 2.38980E-5 * d);
	Saturn.i = rev(2.4886 - 1.081E-7 * d);
	Saturn.w = rev(339.3939 + 2.97661E-5 * d);
	Saturn.a = 9.55475;
	Saturn.e = 0.055546 - 9.499E-9 * d;
	Saturn.M = rev(316.9670 + 0.0334442282 * d);

	// Uranus
	Uranus.N = rev(74.0005 + 1.3978E-5 * d);
	Uranus.i = rev(0.7733 + 1.9E-8 * d);
	Uranus.w = rev(96.6612 + 3.0565E-5 * d);
	Uranus.a = 19.18171 - 1.55E-8 * d;
	Uranus.e = 0.047318 + 7.45E-9 * d;
	Uranus.M = rev(142.5905 + 0.011725806 * d);

	// Neptune
	Neptune.N = rev(131.7806 + 3.0173E-5 * d);
	Neptune.i = rev(1.7700 - 2.55E-7 * d);
	Neptune.w = rev(272.8461 - 6.027E-6 * d);
	Neptune.a = 30.05826 + 3.313E-8 * d;
	Neptune.e = 0.008606 + 2.15E-9 * d;
	Neptune.M = rev(260.2471 + 0.005995147 * d);
}


void sunPos(void){
	double E,x,y,v;
	E = Sun.M + (180/PI) * Sun.e * sind(Sun.M) * (1+Sun.e*cosd(Sun.M));	// Eccentric Anomaly
	x = cosd(E)-Sun.e;									// Calculate Rectangular Coordinates in ecliptic plane of earth with x axis towards perihelion
	y = sind(E) * sqrt(1-Sun.e*Sun.e);
	Sun.r = sqrt(x*x + y*y);						// Calculate Distance from earth
	v = atan2d(y,x);										// Calculate True Anomaly
	Sun.lon = rev(v + Sun.w);								// Calcuate Longitude
	Sun.lat = 0;	// Since it is in the orbital plane then latitude = 0

	Earth.lon = rev(Sun.lon + 180);
	Earth.lat = 0;
	Earth.r = Sun.r;
}

void planetsPos(ORBIT* planet){
	// Iteratively calculate Eccentric Anomaly
	double E0,E1,x,y,v,xeclip,yeclip,zeclip;
  E1=0;
	E0 = planet->M + RADEG * planet->e * sind(planet->M) * (1 + planet->e * cosd(planet->M));
	while(fabs(E1-E0)>0.005){
		E1 = E0 - (E0 - RADEG * planet->e * sind(E0) - planet->M) / (1 - planet->e * cosd(E0));
		E0 = E1;
	}
	// Calculate Rectangular Coordinates in ecliptic plane of planet
	x = planet->a * (cosd(E0) - planet->e);
	y = planet->a * sqrt(1 - planet->e * planet->e) * sind(E0);
	// Calculate Distance and True anomaly
	planet->r = sqrt(x*x + y*y);
	v = atan2d(y,x);
	// Calculate Rectangular Coordinates in ecliptic plane of earth (X-axis points towards vernal equinox)
	xeclip = planet->r * ( cosd(planet->N) * cosd(v+planet->w) - sind(planet->N) * sind(v+planet->w) * cosd(planet->i) );
  yeclip = planet->r * ( sind(planet->N) * cosd(v+planet->w) + cosd(planet->N) * sind(v+planet->w) * cosd(planet->i) );
  zeclip = planet->r * sind(v+planet->w) * sind(planet->i);
	// Convert to Longitude, Latitude and Distance
	planet->lat = asind(zeclip/planet->r);
	planet->lon = rev(atan2d(yeclip,xeclip));
}

void perturbations(void){
	Jupiter.lon +=
	-0.332 * sind(2*Jupiter.M - 5*Saturn.M - 67.6)
  -0.056 * sind(2*Jupiter.M - 2*Saturn.M + 21)
  +0.042 * sind(3*Jupiter.M - 5*Saturn.M + 21)
  -0.036 * sind(Jupiter.M - 2*Saturn.M)
  +0.022 * cosd(Jupiter.M - Saturn.M)
  +0.023 * sind(2*Jupiter.M - 3*Saturn.M + 52)
  -0.016 * sind(Jupiter.M - 5*Saturn.M - 69);

	Saturn.lon +=
	+0.812 * sind(2*Jupiter.M - 5*Saturn.M - 67.6)
  -0.229 * cosd(2*Jupiter.M - 4*Saturn.M - 2)
  +0.119 * sind(Jupiter.M - 2*Saturn.M - 3)
  +0.046 * sind(2*Jupiter.M - 6*Saturn.M - 69)
  +0.014 * sind(Jupiter.M - 3*Saturn.M + 32);

	Saturn.lat +=
	-0.020 * cosd(2*Jupiter.M - 4*Saturn.M - 2)
  +0.018 * sind(2*Jupiter.M - 6*Saturn.M - 49);

	Uranus.lat +=
	+0.040 * sind(Saturn.M - 2*Uranus.M + 6)
  +0.035 * sind(Saturn.M - 3*Uranus.M + 33)
  -0.015 * sind(Jupiter.M - Uranus.M + 20);
}

char* format_I2C(char *bytearray){	// Requires receiver to be little-endian
	// store all longitudes in array as floats
	float planet[8] = {
		(float)Mercury.lon,
		(float)Venus.lon,
		(float)Earth.lon,
		(float)Mars.lon,
		(float)Jupiter.lon,
		(float)Saturn.lon,
		(float)Uranus.lon,
		(float)Neptune.lon
	};
	uint8_t* p = (uint8_t *)&planet;	// Create pointer looking for Bytes with address at start of float array
	for(uint8_t i=0; i<32;i++){				// For all 32 bytes
		bytearray[i] = p[i];						// Copy bytes over in order to bytearray
	}
	return bytearray;
}
