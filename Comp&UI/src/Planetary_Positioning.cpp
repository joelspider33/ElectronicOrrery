/*----------------------------------------------------------------------------
 * Name:    Planetary_Positioning.c
 * Purpose: Planetary Position Computation
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		01/02/2018
 *----------------------------------------------------------------------------*/

#include "Planetary_Positioning.h"
#include "Math.h"
#include "mbed.h"

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


// Main Planet Function
void getPlanetPos(struct planet* PlanetArray, int year, int month, int day, double time){
	double d = ref_date(year,month,day,time);
	struct orbital_elements OrbitArray[8];
	computeOrbitalElements(d,OrbitArray);
	planetsPosCalc(OrbitArray,PlanetArray);
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
	return d1;
}

// Computes initial orbital elements of all planets based on ref date d
void computeOrbitalElements(double d, struct orbital_elements OrbitArray[8]){

	// Mercury
	OrbitArray[0].N = rev(48.3313 + 3.24587E-5 * d);
	OrbitArray[0].i = rev(7.0047 + 5.00E-8 * d);
	OrbitArray[0].w = rev(29.1241 + 1.01444E-5 * d);
	OrbitArray[0].a = 0.387098;
	OrbitArray[0].e = 0.205635 + 5.59E-10 * d;
	OrbitArray[0].M = rev(168.6562 + 4.0923344368 * d);

	// Venus
	OrbitArray[1].N = rev(76.6799 + 2.46590E-5 * d);
	OrbitArray[1].i = rev(3.3946 + 2.75E-8 * d);
	OrbitArray[1].w = rev(54.8910 + 1.38374E-5 * d);
	OrbitArray[1].a = 0.723330;
	OrbitArray[1].e = 0.006773 - 1.302E-9 * d;
	OrbitArray[1].M = rev(48.0052 + 1.6021302244 * d);

	// Sun/-Earth (Refer to word document explaining algoithm for explanation)
	OrbitArray[2].N = 0.0;
	OrbitArray[2].i = 0.0;
	OrbitArray[2].w = rev(282.9404 + 4.70935E-5 * d);
	OrbitArray[2].a = 1.0;
	OrbitArray[2].e = 0.016709 - 1.151E-9 * d;
	OrbitArray[2].M = rev(356.0470 + 0.9856002585 * d);

	// Mars
	OrbitArray[3].N = rev(49.5574 + 2.11081E-5 * d);
  OrbitArray[3].i = rev(1.8497 - 1.78E-8 * d);
  OrbitArray[3].w = rev(286.5016 + 2.92961E-5 * d);
  OrbitArray[3].a = 1.523688;
  OrbitArray[3].e = 0.093405 + 2.516E-9 * d;
  OrbitArray[3].M = rev(18.6021 + 0.5240207766 * d);

	// Jupiter
	OrbitArray[4].N = rev(100.4542 + 2.76854E-5 * d);
	OrbitArray[4].i = rev(1.3030 - 1.557E-7 * d);
	OrbitArray[4].w = rev(273.8777 + 1.64505E-5 * d);
	OrbitArray[4].a = 5.20256;
	OrbitArray[4].e = 0.048498 + 4.469E-9 * d;
	OrbitArray[4].M = rev(19.8950 + 0.0830853001 * d);

	// Saturn
	OrbitArray[5].N = rev(113.6634 + 2.38980E-5 * d);
	OrbitArray[5].i = rev(2.4886 - 1.081E-7 * d);
	OrbitArray[5].w = rev(339.3939 + 2.97661E-5 * d);
	OrbitArray[5].a = 9.55475;
	OrbitArray[5].e = 0.055546 - 9.499E-9 * d;
	OrbitArray[5].M = rev(316.9670 + 0.0334442282 * d);

	// Uranus
	OrbitArray[6].N = rev(74.0005 + 1.3978E-5 * d);
	OrbitArray[6].i = rev(0.7733 + 1.9E-8 * d);
	OrbitArray[6].w = rev(96.6612 + 3.0565E-5 * d);
	OrbitArray[6].a = 19.18171 - 1.55E-8 * d;
	OrbitArray[6].e = 0.047318 + 7.45E-9 * d;
	OrbitArray[6].M = rev(142.5905 + 0.011725806 * d);

	// Neptune
	OrbitArray[7].N = rev(131.7806 + 3.0173E-5 * d);
	OrbitArray[7].i = rev(1.7700 - 2.55E-7 * d);
	OrbitArray[7].w = rev(272.8461 - 6.027E-6 * d);
	OrbitArray[7].a = 30.05826 + 3.313E-8 * d;
	OrbitArray[7].e = 0.008606 + 2.15E-9 * d;
	OrbitArray[7].M = rev(260.2471 + 0.005995147 * d);
}


void planetsPosCalc(struct orbital_elements OrbitArray[8], struct planet PlanetArray[8]){
	for (int i=0; i<8; i++)
	{
		if(i==2)
		{
			double E,x,y,v;
			E = OrbitArray[i].M + (180/PI) * OrbitArray[i].e * sind(OrbitArray[i].M) * (1+OrbitArray[i].e*cosd(OrbitArray[i].M));	// Eccentric Anomaly
			x = cosd(E)-OrbitArray[i].e;									// Calculate Rectangular Coordinates in ecliptic plane of earth with x axis towards perihelion
			y = sind(E) * sqrt(1-OrbitArray[i].e*OrbitArray[i].e);
			v = atan2d(y,x);										// Calculate True Anomaly

			PlanetArray[i].lon = rev(v + OrbitArray[i].w + 180);
			PlanetArray[i].lat = 0;
			PlanetArray[i].r = sqrt(x*x + y*y);
		}
		else
		{
			struct orbital_elements orbit = OrbitArray[i];
			struct planet planet = PlanetArray[i];
			// Iteratively calculate Eccentric Anomaly
			double E0,E1,x,y,v,xeclip,yeclip,zeclip;
		  E1=0;
			E0 = orbit.M + RADEG * orbit.e * sind(orbit.M) * (1 + orbit.e * cosd(orbit.M));
			while(fabs(E1-E0)>0.005){
				E1 = E0 - (E0 - RADEG * orbit.e * sind(E0) - orbit.M) / (1 - orbit.e * cosd(E0));
				E0 = E1;
			}
			// Calculate Rectangular Coordinates in ecliptic plane of planet
			x = orbit.a * (cosd(E0) - orbit.e);
			y = orbit.a * sqrt(1 - orbit.e * orbit.e) * sind(E0);
			// Calculate Distance and True anomaly
			planet.r = sqrt(x*x + y*y);
			v = atan2d(y,x);
			// Calculate Rectangular Coordinates in ecliptic plane of earth (X-axis points towards vernal equinox)
			xeclip = planet.r * ( cosd(orbit.N) * cosd(v+orbit.w) - sind(orbit.N) * sind(v+orbit.w) * cosd(orbit.i) );
		  yeclip = planet.r * ( sind(orbit.N) * cosd(v+orbit.w) + cosd(orbit.N) * sind(v+orbit.w) * cosd(orbit.i) );
		  zeclip = planet.r * sind(v+orbit.w) * sind(orbit.i);
			// Convert to Longitude, Latitude and Distance
			planet.lat = asind(zeclip/planet.r);
			planet.lon = rev(atan2d(yeclip,xeclip));
			PlanetArray[i] = planet;
		}
	}

	// Adding On Perterbations between major planets
	struct orbital_elements Jupiter = OrbitArray[4];
	struct orbital_elements Saturn = OrbitArray[5];
	struct orbital_elements Uranus = OrbitArray[6];

	PlanetArray[4].lon +=
	-0.332 * sind(2*Jupiter.M - 5*Saturn.M - 67.6)
	-0.056 * sind(2*Jupiter.M - 2*Saturn.M + 21)
	+0.042 * sind(3*Jupiter.M - 5*Saturn.M + 21)
	-0.036 * sind(Jupiter.M - 2*Saturn.M)
	+0.022 * cosd(Jupiter.M - Saturn.M)
	+0.023 * sind(2*Jupiter.M - 3*Saturn.M + 52)
	-0.016 * sind(Jupiter.M - 5*Saturn.M - 69);

	PlanetArray[5].lon +=
	+0.812 * sind(2*Jupiter.M - 5*Saturn.M - 67.6)
	-0.229 * cosd(2*Jupiter.M - 4*Saturn.M - 2)
	+0.119 * sind(Jupiter.M - 2*Saturn.M - 3)
	+0.046 * sind(2*Jupiter.M - 6*Saturn.M - 69)
	+0.014 * sind(Jupiter.M - 3*Saturn.M + 32);

	PlanetArray[5].lat +=
	-0.020 * cosd(2*Jupiter.M - 4*Saturn.M - 2)
	+0.018 * sind(2*Jupiter.M - 6*Saturn.M - 49);

	PlanetArray[6].lat +=
	+0.040 * sind(Saturn.M - 2*Uranus.M + 6)
	+0.035 * sind(Saturn.M - 3*Uranus.M + 33)
	-0.015 * sind(Jupiter.M - Uranus.M + 20);

 	OrbitArray[4] = Jupiter;
	OrbitArray[5] = Saturn;
	OrbitArray[6] = Uranus;

	return;
}
