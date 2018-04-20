/*----------------------------------------------------------------------------
 * Name:    Planetary_Positioning.h
 * Purpose: Planetary Position Computation Header File
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		01/02/2018
 *----------------------------------------------------------------------------*/

 struct planet{
 	double lon;		// Longitude
 	double lat;		// Latitude
 	double r;			// Distance from sun
 };
 
struct orbital_elements{
	double N;		// Longitude of Ascending Node (degrees)
	double i;		// Inclination to the ecliptic (degrees)
	double w;		// Argument of Perihelion (degrees)
	double a;		// Semi-Major Axis (AU) - Astronomical Units
	double e;		// Eccentricity (unitless)
	double M;		// Mean Anomaly (degrees)
};

double rev( double x );
double ref_date(int year,int month,int date,double time);
void getPlanetPos(struct planet* PlanetArray,int year,int month, int date, double time);
void computeOrbitalElements(double d, struct orbital_elements OrbitArray[8]);
void planetsPosCalc(struct orbital_elements OrbitArray[8], struct planet PlanetArray[8]);
