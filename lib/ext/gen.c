////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     gen.c                                                            //
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

#include <ruby.h>

extern VALUE rb_cRandom;

/* the random number generator */
static VALUE rb_RandGen = Qnil;

#define GET_RB_RANF	rb_funcall(rb_RandGen, rb_intern("rand"), 0)
double ranf(void)
{
	return NUM2DBL(GET_RB_RANF);
}

VALUE rb_ranf(void)
{
	return GET_RB_RANF;
}
#undef GET_RB_RANF

VALUE rv_gen_get_seed(void)
{
	return rb_funcall(rb_RandGen, rb_intern("seed"), 0);
}

void rv_gen_set_seed(VALUE rb_seed)
{
	rb_RandGen = rb_funcall(rb_cRandom, rb_intern("new"), 1, rb_seed);
}

/* Must be called before any kind ranf() call */
void rv_init_gen(void)
{
	rb_RandGen = rb_funcall(rb_cRandom, rb_intern("new"), 0);

	/* tell the GC not to collect the random generator */
	rb_gc_register_address(&rb_RandGen);
}
