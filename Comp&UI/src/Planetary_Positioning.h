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

// rev() normalises and input angle to between 0° and 360°
double rev( double x );
// getPlanetPos() returns calculated planet struct data to PlanetArray. (Calls ref_date(),computeOrbitalElements(),planetsPosCalc())
void getPlanetPos(struct planet* PlanetArray,int year,int month, int date, double time);
// ref_date() calculates and returns the value of the julian date d
double ref_date(int year,int month,int date,double time);
// computeOrbitalElements() stores orbital_elements struct data calculated from julian date d
void computeOrbitalElements(double d, struct orbital_elements OrbitArray[8]);
// planetsPosCalc() calculated planet struct data from orbital_elements struct data using formulae
void planetsPosCalc(struct orbital_elements OrbitArray[8], struct planet PlanetArray[8]);
