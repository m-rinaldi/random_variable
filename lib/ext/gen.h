////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     gen.h                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Author:   Jorge F.M. Rinaldi                                               //
// Contact:  jorge.madronal.rinaldi@gmail.com                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Date:     2012/12/01                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef __GEN_H__
#define __GEN_H__

#include <ruby.h>

void rv_init_gen(void);
double ranf(void);
VALUE rv_gen_new_seed(void);
void rv_gen_set_seed(VALUE);
VALUE rv_gen_get_seed(void);

#endif /* __GEN_H__ */
