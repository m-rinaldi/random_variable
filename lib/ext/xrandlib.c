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


