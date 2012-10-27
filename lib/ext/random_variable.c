////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     random_variable.c                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Author:   Jorge F.M. Rinaldi                                               //
// Contact:  jorge.madronal.rinaldi@gmail.com                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Date:     2012/10/11                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_RUBY_H
#include <ruby.h>
#else 
#error "Need ruby.h header"
#endif /* HAVE_RUBY_H */

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error "Need math.h header"
#endif /* HAVE_MATH_H */ 

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#error "Need limits.h header"
#endif /* HAVE_LIMITS_H */

#include "randlib.h"
#include "xrandlib.h"

/******************************************************************************/
/* class objects */
/******************************************************************************/
static VALUE rb_cRandomVariable;
static VALUE rb_cBernoulliRV;
static VALUE rb_cPoissonRV;
static VALUE rb_cNormalRV;
static VALUE rb_cExponentialRV;

/******************************************************************************/
/* Common Functions */
/******************************************************************************/
#define CAST(f) ((VALUE (*)(void *))(f))

static inline void check_not_infinite(double x, const char *str)
{
	if (isfinite(x))
		return;
	rb_raise(rb_eArgError, "%s", str);
}

static inline void check_not_nan(double x, const char *str)
{
	if (isnan(x))
		rb_raise(rb_eArgError, "%s", str);
}


static inline long get_times(VALUE times)
{
	long nr_times;
	Check_Type(times, T_FIXNUM);
	if ( (nr_times = NUM2LONG(times)) < 0)
		rb_raise(rb_eArgError, "negative number of outcomes");	
	return nr_times;
}

static inline VALUE
_RV_outcomes(void *rv, VALUE (*_RV_outcome)(void *rv), VALUE times)
{
	long repeat;
	VALUE outcomes_ary;

	repeat = get_times(times);

	outcomes_ary = rb_ary_new();
	for (; repeat > 0; --repeat)
		rb_ary_push(outcomes_ary, _RV_outcome(rv));	
	return outcomes_ary;
}

/******************************************************************************/
/* Bernoulli Random Variable */
/******************************************************************************/
typedef struct {
	double p;
} bernoulli_rv_t;

bernoulli_rv_t *bernoulli_rv_create(double p)
{
	bernoulli_rv_t *rv;

	check_not_nan(p, "not a number (NaN) p parameter");
	check_not_infinite(p, "infinite p parameter");

	/* Check p parameter */	
	if (p <= 0.0 || p >= 1.0) {
		rb_raise(rb_eArgError, "wrong parameter value (0 < p < 1)");
		return NULL;	
	}

	/* p parameter correct */	
	rv = ALLOC(bernoulli_rv_t);
	rv->p = p;
	return rv;
}

VALUE BernoulliRV_new(VALUE self, VALUE p)
{
	bernoulli_rv_t *rv;
	VALUE rb_obj; 

	rv = bernoulli_rv_create(NUM2DBL(p));
	rb_obj = Data_Wrap_Struct(rb_cBernoulliRV, NULL, xfree, rv);
	return rb_obj;	
}

static inline bernoulli_rv_t *_BernoulliRV(VALUE BernoulliRV_obj)
{
	bernoulli_rv_t *rv;
	Data_Get_Struct(BernoulliRV_obj, bernoulli_rv_t, rv);
	return rv;
}

VALUE BernoulliRV_p(VALUE self)
{
	return rb_float_new(_BernoulliRV(self)->p);
}

static inline VALUE _BernoulliRV_outcome(bernoulli_rv_t *rv)
{
	return INT2FIX(genbern(rv->p));
}

VALUE BernoulliRV_outcome(VALUE self)
{
	return _BernoulliRV_outcome(_BernoulliRV(self));	
}

VALUE BernoulliRV_outcomes(VALUE self, VALUE times)
{
	bernoulli_rv_t *rv;
	
	rv = _BernoulliRV(self);
	return _RV_outcomes(rv, CAST(_BernoulliRV_outcome), times);	
}

/******************************************************************************/
/* Poisson Random Variable */
/******************************************************************************/
typedef struct {
	double lambda;
} poisson_rv_t;

#define POISSON_LAMBDA_MAX (LONG_MAX - (long)(0.00001*LONG_MAX))

static inline poisson_rv_t *poisson_rv_create(double lambda)
{
	poisson_rv_t *rv;

	check_not_nan(lambda, "not a number (NaN) lambda parameter");
	check_not_infinite(lambda, "infinite lambda parameter");

	/* Lambda has to be positive */
	if (lambda <= 0.0) {
		rb_raise(rb_eArgError, "non-positive lambda parameter");
		return NULL;
	}

	/* Chech lambda parameter limits */
	if (lambda > POISSON_LAMBDA_MAX) {
		rb_raise(rb_eArgError, "too high lambda parameter");
		return NULL;
	}

	/* Lambda parameter correct */
	rv = ALLOC(poisson_rv_t);
	rv->lambda = lambda;
	return rv;
}

VALUE PoissonRV_new(VALUE self, VALUE lambda)
{
	poisson_rv_t *rv;
	VALUE obj; 
	
	rv = poisson_rv_create(NUM2DBL(lambda));
	obj = Data_Wrap_Struct(rb_cPoissonRV, NULL, xfree, rv);
	return obj;	
}

static inline poisson_rv_t *_PoissonRV(VALUE PoissonRV_obj)
{
	poisson_rv_t *rv;
	Data_Get_Struct(PoissonRV_obj, poisson_rv_t, rv);
	return rv;
}

VALUE PoissonRV_max_lambda(VALUE self)
{
	return DBL2NUM(POISSON_LAMBDA_MAX);	
}

VALUE PoissonRV_lambda(VALUE self)
{
	return rb_float_new(_PoissonRV(self)->lambda);
}

static inline VALUE _PoissonRV_outcome(poisson_rv_t *rv)
{
	return LONG2NUM(ignpoi(rv->lambda));
}

VALUE PoissonRV_outcome(VALUE self)
{
	return _PoissonRV_outcome(_PoissonRV(self));
}

VALUE PoissonRV_outcomes(VALUE self, VALUE times)
{
	poisson_rv_t *rv;
	
	rv = _PoissonRV(self);
	return _RV_outcomes(rv, CAST(_PoissonRV_outcome), times);
}


/******************************************************************************/
/* Normal Random Variable */
/******************************************************************************/
typedef struct {
	double mu;
	double sigma;
} normal_rv_t;

static inline normal_rv_t *normal_rv_create(double mu, double sigma)
{
	normal_rv_t *rv;

	if (sigma <= 0.0) {
		rb_raise(rb_eArgError, "non-positive sigma parameter");
		return NULL;
	}
	
	rv = ALLOC(normal_rv_t);
	rv->mu = mu;
	rv->sigma = sigma;
	return rv;
}

VALUE NormalRV_new(VALUE self, VALUE mu, VALUE sigma)
{
	normal_rv_t *rv;
	VALUE rb_obj;

	rv = normal_rv_create(NUM2DBL(mu), NUM2DBL(sigma));
	rb_obj = Data_Wrap_Struct(rb_cNormalRV, NULL, xfree, rv);
	return rb_obj;	
}

static inline normal_rv_t *_NormalRV(VALUE NormalRV_obj)
{
	normal_rv_t *rv;
	Data_Get_Struct(NormalRV_obj, normal_rv_t, rv);
	return rv;
}

VALUE NormalRV_mu(VALUE self)
{
	normal_rv_t *rv;
	return _NormalRV(self)->mu;
}

VALUE NormalRV_sigma(VALUE self)
{
	normal_rv_t *rv;
	return _NormalRV(self)->sigma;
}

static VALUE _NormalRV_outcome(normal_rv_t *rv)
{
	return rb_float_new(
		gennor(rv->mu, rv->sigma)
	);
}

VALUE NormalRV_outcome(VALUE self)
{
	return _NormalRV_outcome(_NormalRV(self));
}

VALUE NormalRV_outcomes(VALUE self, VALUE times)
{
	normal_rv_t *rv;
	rv = _NormalRV(self);
	return _RV_outcomes(rv, CAST(_NormalRV_outcome), times);
}


/******************************************************************************/
/* Exponential Random Variable */
/******************************************************************************/
typedef struct {
	double mean; 
} exponential_rv_t;

static inline exponential_rv_t *exponential_rv_create(double lambda)
{
	exponential_rv_t *rv;

	if (lambda <= 0) {
		rb_raise(rb_eArgError, "non-positive parameter value");
		return NULL;
	}

	rv = ALLOC(exponential_rv_t);
	
	rv->mean = 1.0 / lambda;
	return rv;
}

VALUE ExponentialRV_new(VALUE self, VALUE lambda)
{
	exponential_rv_t *rv;
	VALUE ExponentialRV_obj;

	rv = exponential_rv_create(NUM2DBL(lambda));
	ExponentialRV_obj = Data_Wrap_Struct(rb_cExponentialRV, NULL, xfree, rv);
	return ExponentialRV_obj;	
}

static inline exponential_rv_t *_ExponentialRV(VALUE ExponentialRV_obj)
{
	exponential_rv_t *rv;
	Data_Get_Struct(ExponentialRV_obj, exponential_rv_t, rv);
	return rv;
}

VALUE ExponentialRV_lambda(VALUE self)
{
	return 1.0 / _ExponentialRV(self)->mean;
}

static inline VALUE _ExponentialRV_outcome(exponential_rv_t *rv)
{
	return rb_float_new(genexp(rv->mean));
}

VALUE ExponentialRV_outcome(VALUE self)
{
	return _ExponentialRV_outcome(_ExponentialRV(self));
}

VALUE ExponentialRV_outcomes(VALUE self, VALUE times)
{
	exponential_rv_t *rv;
	
	rv = _ExponentialRV(self);
	return _RV_outcomes(rv, CAST(_ExponentialRV_outcome), times);
}


/******************************************************************************/
/* Extension Initialization */
/******************************************************************************/
void Init_random_variable(void)
{
	/* RandomVariable */
	rb_cRandomVariable = rb_define_class("RandomVariable", rb_cObject);

	/* BernoulliRV */
	rb_cBernoulliRV = rb_define_class("BernoulliRV", rb_cRandomVariable);
	rb_define_singleton_method(rb_cBernoulliRV, "new", BernoulliRV_new, 1);
	rb_define_method(rb_cBernoulliRV, "p", BernoulliRV_p, 0);
	rb_define_method(rb_cBernoulliRV, "outcome", BernoulliRV_outcome, 0);
	rb_define_method(rb_cBernoulliRV, "outcomes", BernoulliRV_outcomes, 1);

	/* PoissonRV */
	rb_cPoissonRV = rb_define_class("PoissonRV", rb_cRandomVariable);
	rb_define_singleton_method(rb_cPoissonRV, "new", PoissonRV_new, 1);
	rb_define_singleton_method(rb_cPoissonRV, 
			"lambda_max", PoissonRV_max_lambda, 0);
	rb_define_method(rb_cPoissonRV, "lambda", PoissonRV_lambda, 0);	
	rb_define_method(rb_cPoissonRV, "mean", PoissonRV_lambda, 0);	
	rb_define_method(rb_cPoissonRV, "var", PoissonRV_lambda, 0);	
	rb_define_method(rb_cPoissonRV, "outcome", PoissonRV_outcome, 0);	
	rb_define_method(rb_cPoissonRV, "outcomes", PoissonRV_outcomes, 1);

	/* NormalRV */
	rb_cNormalRV = rb_define_class("NormalRV", rb_cRandomVariable);
	rb_define_singleton_method(rb_cNormalRV, "new", NormalRV_new, 2);
	rb_define_method(rb_cNormalRV, "outcome", NormalRV_outcome, 0);	
	rb_define_method(rb_cNormalRV, "outcomes", NormalRV_outcomes, 1);

	/* ExponentialRV */
	rb_cExponentialRV = rb_define_class("ExponentialRV", 
						rb_cRandomVariable);
	rb_define_singleton_method(rb_cExponentialRV, "new", 
						ExponentialRV_new, 1);
	rb_define_method(rb_cExponentialRV, "lambda", ExponentialRV_lambda, 0);
	rb_define_method(rb_cExponentialRV, "rate", ExponentialRV_lambda, 0);
	rb_define_method(rb_cExponentialRV, "outcome", 
						ExponentialRV_outcome, 0);
	rb_define_method(rb_cExponentialRV, "outcomes", 
						ExponentialRV_outcomes, 1);
		
	return;
}
