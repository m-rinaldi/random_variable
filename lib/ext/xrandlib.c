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

/* Arcsine */
double gen_arcsine(void)
{
	return pow(sin(M_2_PI * ranf() ), 2);
}

/* Bates */
double gen_bates(long n)
{
	return genirwinhall(n) / n;
}

/* Bernoulli */
int gen_bernoulli(double p)
{
	if (ranf() > p)
		return 0;
	return 1;
}

/* Chi-Squared */
double gen_chi_squared(long k)
{
	return genchi((double) k);
}

/* Discrete Uniform */
long gen_discrete_uniform(long a, long b)
{
	/* assumptions:
		1) a != b
		2) a < b
	*/
	double step;
	long step_nr;
	double y, y0;

	step = 1.0 / (b - a + 1);
	y = step;
	y0 = ranf();
	step_nr = 0;

	do {
		if (y0 <= y)
			break;
		y += step;
		++step_nr;	
	} while (1);
	return a + step_nr;	
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

/* Rademacher */
int gen_rademacher(void)
{
	if (ranf() <= 0.5)
		return -1;
	return 1;	
}

/* Rayleigh */
double gen_rayleigh(double sigma)
{
	return sigma * sqrt(-2 * log(1.0 - ranf()));
}

/* Pareto */
double gen_pareto(double a, double m)
{
	return m * pow(1 - ranf(), 1.0 / a);
}

/* Rectangular */
double gen_rectangular(void)
{
	return (ranf() - 0.5);	
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


