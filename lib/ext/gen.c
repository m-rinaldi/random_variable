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

/* The Ruby "Random" class */
extern VALUE rb_cRandom;

/* the random number generator (instance) */
static VALUE rb_RandGen = Qnil;

/* For returning a random number at the C level */
#define GET_RB_RANF	rb_funcall(rb_RandGen, rb_intern("rand"), 0)
double ranf(void)
{
	return NUM2DBL(GET_RB_RANF);
}

/* For returning a random number at the Ruby level */
VALUE rb_ranf(void)
{
	return GET_RB_RANF;
}
#undef GET_RB_RANF

/******************************************************************************/
/* Functions at C level	*/
/******************************************************************************/

/* get the current seed */
VALUE rv_gen_get_seed(void)
{
	return rb_funcall(rb_RandGen, rb_intern("seed"), 0);
}

/* set the current seed */
void rv_gen_set_seed(VALUE rb_seed)
{
	/* memory leak?! */
	rb_RandGen = rb_funcall(rb_cRandom, rb_intern("new"), 1, rb_seed);
}

VALUE rv_get_new_seed(void)
{
	rb_RandGen = rb_funcall(rb_cRandom, rb_intern("new"), 0);
	return rv_gen_get_seed();
}
/******************************************************************************/


/* Must be called BEFORE any kind ranf() call !! */
void rv_init_gen(void)
{
	/* create an instance of the random generator */
	rv_get_new_seed();

	/* tell the GC not to collect the random generator */
	rb_gc_register_address(&rb_RandGen);
}
