////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     xrandlib.c                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Author:   Jorge F.M. Rinaldi                                               //
// Contact:  jorge.madronal.rinaldi@gmail.com                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Date:     2012/10/12                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error "Need math.h header"
#endif /* HAVA_MATH_H */

#include "xrandlib.h"
#include "randlib.h"

/* Bernoulli */
int genbern(double p)
{
	if (ranf() > p)
		return 0;
	return 1;
}

/* Rayleigh */
double genray(double sigma)
{
	return sqrt( -2 * sigma * sigma * log(1.0 - ranf()) );
}
