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

/*******************************************************************************
    random_variable gem for the creation or random variables in Ruby
    Copyright (C) 2012 Jorge Fco. Madronal Rinaldi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error "No math.h header found"
#endif /* HAVE_MATH_H */

#include "xrandlib.h"
#include "randlib.h"

int gen_bernoulli(double p)
{
	if (ranf() > p)
		return 0;
	return 1;
}

double gen_exponential(double lambda)
{
	return genexp(1.0 / lambda);
}

/* Rayleigh */
double gen_rayleigh(double sigma)
{
	return sqrt( -2 * sigma * sigma * log(1.0 - ranf()) );
}

/* Arcsine */
double gen_arcsine(void)
{
	return pow(sin(M_2_PI * ranf() ), 2);
}

/* Rectangular */
double gen_rectangular(void)
{
	return (ranf() - 0.5);	
}

/* Irwin-Hall */
double gen_irwin_hall(long n)
{
	long i;
	double sum;
	for (i = 0, sum = 0; i < n; i++)
		sum += ranf();
	return sum;	
}

/* Bates */
double gen_bates(long n)
{
	return genirwinhall(n) / n;
}

/* Triangular */
double gen_triangular(double a, double b, double c)
{
	double y = ranf();
	if (y <= (c-a) / (b-a) )
		return a + sqrt(y*(b-a)*(c-a));
	return b - sqrt((1-y)*(b-a)*(b-c));
}

/* Truncated Normal */
double gen_truncated_normal(double mu, double sigma, double a, double b)
{
	return 0.0;	
}


