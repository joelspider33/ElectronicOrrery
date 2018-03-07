/*----------------------------------------------------------------------------
 * Name:    Planetary_Positioning.h
 * Purpose: Planetary Position Computation Header File
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		01/02/2018
 *----------------------------------------------------------------------------*/

#ifndef PLANET_HEADER_FILE
#define PLANET_HEADER_FILE

#include "stdint.h"

typedef struct orbital_elements{
	double N;		// Longitude of Ascending Node (degrees)
	double i;		// Inclination to the ecliptic (degrees)
	double w;		// Argument of Perihelion (degrees)
	double a;		// Semi-Major Axis (AU) - Astronomical Units
	double e;		// Eccentricity (unitless)
	double M;		// Mean Anomaly (degrees)
	double lon;		// Longitude
	double lat;		// Latitude
	double r;			// Distance from sun
} ORBIT;


double rev( double x );
double ref_date(int year,int month,int date,double time);
void PosComp(int year,int month, int date, double time);
void compute_orbital_elements(void);
void sunPos(void);
void planetsPos(ORBIT* planet);
void perturbations(void);
char* format_I2C(char *bytearray);

#endif
