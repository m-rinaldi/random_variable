////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     xrandlib.h                                                       //
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


#ifndef _XRANDLIB_H_
#define _XRANDLIB_H_

extern int 	gen_bernoulli(double);
extern double 	gen_chi_squared(long);
extern long	gen_discrete_uniform(long a, long b);
extern double 	gen_exponential(double);
extern int 	gen_rademacher(void);
extern double 	gen_rayleigh(double);

#endif /* _XRANDLIB_H_ */
