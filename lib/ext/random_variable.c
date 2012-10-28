////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     random_variable.c                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Author:   Jorge F.M. Rinaldi                                               //
// Contact:  jorge.madronal.rinaldi@gmail.com                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Date:     2012/10/11                                                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
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

#ifdef HAVE_RUBY_H
#include <ruby.h>
#else 
#error "No ruby.h header found"
#endif /* HAVE_RUBY_H */

#ifdef HAVE_MATH_H
#include <math.h>
#else
#error "No math.h header found"
#endif /* HAVE_MATH_H */ 

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#error "No limits.h header found"
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
static VALUE rb_cRayleighRV;
static VALUE rb_cContinuousUniformRV;
static VALUE rb_cDiscreteUniformRV;
/******************************************************************************/
/* Common Functions */
/******************************************************************************/
#define CAST(f) ((VALUE (*)(void *))(f))

static inline void check_not_nan(double x, const char *str)
{
	if (isnan(x))
		rb_raise(rb_eArgError, "%s", str);
}

#define CHECK_NOT_NAN(x) check_not_nan((x),"not a number " #x " parameter")

static inline void check_not_infinite(double x, const char *str)
{
	if (isfinite(x))
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define CHECK_NOT_INFINITE(x) check_not_infinite((x),"infinite " #x " paramter")

static inline void check_positive(double x, const char *str)
{
	if (x > 0.0)
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define CHECK_POSITIVE(x) check_positive((x), "non-positive " #x " parameter")

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

	CHECK_NOT_NAN(p);
	CHECK_NOT_INFINITE(p);

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

	/* Check lambda parameter limits */
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

	check_not_nan(mu, "not a number (NaN) mu parameter");
	check_not_infinite(mu, "infinite mu parameter");

	check_not_nan(sigma, "not a number (NaN) sigma parameter");
	check_not_infinite(sigma, "infinite sigma parameter");

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

	check_not_nan(lambda, "not a number (NaN) lambda parameter");
	check_not_infinite(lambda, "infinite lambda parameter");

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
	ExponentialRV_obj = 
			Data_Wrap_Struct(rb_cExponentialRV, NULL, xfree, rv);
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
	return DBL2NUM(1.0 / _ExponentialRV(self)->mean);
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
/* Rayleigh Random Variable */
/******************************************************************************/
typedef struct {
	double sigma;
} rayleigh_rv_t;

static inline rayleigh_rv_t *rayleigh_rv_create(double sigma)
{
	rayleigh_rv_t *rv;

	check_not_nan(sigma, "not a number (NaN) sigma parameter");
	check_not_infinite(sigma, "infinite sigma parameter");

	if (sigma <= 0) {
		rb_raise(rb_eArgError, "non-positive parameter value");
		return NULL;
	}

	rv = ALLOC(rayleigh_rv_t);
	
	rv->sigma = sigma;
	return rv;
}

VALUE RayleighRV_new(VALUE self, VALUE sigma)
{
	rayleigh_rv_t *rv;
	VALUE RayleighRV_obj;

	rv = rayleigh_rv_create(NUM2DBL(sigma));
	RayleighRV_obj = 
			Data_Wrap_Struct(rb_cRayleighRV, NULL, xfree, rv);
	return RayleighRV_obj;	
}

static inline rayleigh_rv_t *_RayleighRV(VALUE RayleighRV_obj)
{
	rayleigh_rv_t *rv;
	Data_Get_Struct(RayleighRV_obj, rayleigh_rv_t, rv);
	return rv;
}

VALUE RayleighRV_sigma(VALUE self)
{
	return DBL2NUM(_RayleighRV(self)->sigma);
}

static inline VALUE _RayleighRV_outcome(rayleigh_rv_t *rv)
{
	return rb_float_new(genray(rv->sigma));
}

VALUE RayleighRV_outcome(VALUE self)
{
	return _RayleighRV_outcome(_RayleighRV(self));
}

VALUE RayleighRV_outcomes(VALUE self, VALUE times)
{
	rayleigh_rv_t *rv;
	
	rv = _RayleighRV(self);
	return _RV_outcomes(rv, CAST(_RayleighRV_outcome), times);
}

/******************************************************************************/
/* Continuous Uniform Random Variable */
/******************************************************************************/
typedef struct {
	double a;
	double b;
} continuous_uniform_rv_t;

static inline
continuous_uniform_rv_t *continuous_uniform_rv_create(double a, double b)
{
	continuous_uniform_rv_t *rv;

	check_not_nan(a, "not a number (NaN) sigma parameter");
	check_not_infinite(a, "infinite sigma parameter");

	check_not_nan(b, "not a number (NaN) sigma parameter");
	check_not_infinite(b, "infinite sigma parameter");

	if (a >= b) {
		rb_raise(rb_eArgError, "a < b");
		return NULL;
	}

	rv = ALLOC(continuous_uniform_rv_t);
	
	rv->a = a;
	rv->b = b;
	return rv;
}

VALUE ContinuousUniformRV_new(VALUE self, VALUE a, VALUE b)
{
	continuous_uniform_rv_t *rv;
	VALUE ContinuousUniformRV_obj;

	rv = continuous_uniform_rv_create(NUM2DBL(a), NUM2DBL(b));
	ContinuousUniformRV_obj = 
		Data_Wrap_Struct(rb_cContinuousUniformRV, NULL, xfree, rv);
	return ContinuousUniformRV_obj;	
}

static inline 
continuous_uniform_rv_t *_ContinuousUniformRV(VALUE ContinuousUniformRV_obj)
{
	continuous_uniform_rv_t *rv;
	Data_Get_Struct(ContinuousUniformRV_obj, continuous_uniform_rv_t, rv);
	return rv;
}

VALUE ContinuousUniformRV_a(VALUE self)
{
	return DBL2NUM(_ContinuousUniformRV(self)->a);
}

VALUE ContinuousUniformRV_b(VALUE self)
{
	return DBL2NUM(_ContinuousUniformRV(self)->b);
}

static inline VALUE _ContinuousUniformRV_outcome(continuous_uniform_rv_t *rv)
{
	return rb_float_new(genunf(rv->a, rv->b));
}

VALUE ContinuousUniformRV_outcome(VALUE self)
{
	return _ContinuousUniformRV_outcome(_ContinuousUniformRV(self));
}

VALUE ContinuousUniformRV_outcomes(VALUE self, VALUE times)
{
	continuous_uniform_rv_t *rv;
	
	rv = _ContinuousUniformRV(self);
	return _RV_outcomes(rv, CAST(_ContinuousUniformRV_outcome), times);
}

/******************************************************************************/
/* Discrete Uniform Random Variable */
/******************************************************************************/
typedef struct {
	long a;
	long b;
} discrete_uniform_rv_t;

static inline
discrete_uniform_rv_t *discrete_uniform_rv_create(long a, long b)
{
	discrete_uniform_rv_t *rv;

	check_not_nan(a, "not a number (NaN) sigma parameter");
	check_not_infinite(a, "infinite sigma parameter");

	check_not_nan(b, "not a number (NaN) sigma parameter");
	check_not_infinite(b, "infinite sigma parameter");

	if (a >= b) {
		rb_raise(rb_eArgError, "a < b");
		return NULL;
	}

	rv = ALLOC(discrete_uniform_rv_t);
	
	rv->a = a;
	rv->b = b;
	return rv;
}

VALUE DiscreteUniformRV_new(VALUE self, VALUE a, VALUE b)
{
	discrete_uniform_rv_t *rv;
	VALUE DiscreteUniformRV_obj;

	/* Check both parameters are integers only */
	VALUE a_is_integer, b_is_integer;
	a_is_integer = rb_obj_is_kind_of(a, rb_cInteger);
	b_is_integer = rb_obj_is_kind_of(b, rb_cInteger);
	
	if (!a_is_integer && b_is_integer)
		rb_raise(rb_eArgError, "not integer a parameter");
	else if (a_is_integer && !b_is_integer)
		rb_raise(rb_eArgError, "not integer b parameter");
	else if (!a_is_integer && !b_is_integer)
		rb_raise(rb_eArgError, "not integer a and b  parameters");
		

	rv = discrete_uniform_rv_create(NUM2LONG(a), NUM2LONG(b));
	DiscreteUniformRV_obj = 
		Data_Wrap_Struct(rb_cDiscreteUniformRV, NULL, xfree, rv);
	return DiscreteUniformRV_obj;	
}

static inline 
discrete_uniform_rv_t *_DiscreteUniformRV(VALUE DiscreteUniformRV_obj)
{
	discrete_uniform_rv_t *rv;
	Data_Get_Struct(DiscreteUniformRV_obj, discrete_uniform_rv_t, rv);
	return rv;
}

VALUE DiscreteUniformRV_a(VALUE self)
{
	return NUM2LONG(_DiscreteUniformRV(self)->a);
}

VALUE DiscreteUniformRV_b(VALUE self)
{
	return LONG2NUM(_DiscreteUniformRV(self)->b);
}

static inline VALUE _DiscreteUniformRV_outcome(discrete_uniform_rv_t *rv)
{
	return rb_float_new(genunf(rv->a, rv->b));
}

VALUE DiscreteUniformRV_outcome(VALUE self)
{
	return _DiscreteUniformRV_outcome(_DiscreteUniformRV(self));
}

VALUE DiscreteUniformRV_outcomes(VALUE self, VALUE times)
{
	discrete_uniform_rv_t *rv;
	
	rv = _DiscreteUniformRV(self);
	return _RV_outcomes(rv, CAST(_DiscreteUniformRV_outcome), times);
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

	/* Rayleigh */
	rb_cRayleighRV = rb_define_class("RayleighRV", rb_cRandomVariable);
	rb_define_singleton_method(rb_cRayleighRV, "new",
						RayleighRV_new, 1);
	rb_define_method(rb_cRayleighRV, "sigma", RayleighRV_sigma, 0);
	rb_define_method(rb_cRayleighRV, "outcome", 
						RayleighRV_outcome, 0);
	rb_define_method(rb_cRayleighRV, "outcomes", 
						RayleighRV_outcomes, 1);

	/* Continuous Uniform */
	rb_cContinuousUniformRV = 
		rb_define_class("ContinuousUniformRV", rb_cRandomVariable);
	rb_define_singleton_method(rb_cContinuousUniformRV, "new",
				ContinuousUniformRV_new, 2);
	rb_define_method(rb_cContinuousUniformRV, 
				"a", ContinuousUniformRV_a, 0);
	rb_define_method(rb_cContinuousUniformRV, 
				"b", ContinuousUniformRV_b, 0);
	rb_define_method(rb_cContinuousUniformRV, "outcome", 
				ContinuousUniformRV_outcome, 0);
	rb_define_method(rb_cContinuousUniformRV, "outcomes", 
				ContinuousUniformRV_outcomes, 1);

	/* Discrete Uniform */
	rb_cDiscreteUniformRV = 
		rb_define_class("DiscreteUniformRV", rb_cRandomVariable);
	rb_define_singleton_method(rb_cDiscreteUniformRV, "new",
				DiscreteUniformRV_new, 2);
	rb_define_method(rb_cDiscreteUniformRV, 
				"a", DiscreteUniformRV_a, 0);
	rb_define_method(rb_cDiscreteUniformRV, 
				"b", DiscreteUniformRV_b, 0);
	rb_define_method(rb_cDiscreteUniformRV, "outcome", 
				DiscreteUniformRV_outcome, 0);
	rb_define_method(rb_cDiscreteUniformRV, "outcomes", 
				DiscreteUniformRV_outcomes, 1);
	
	return;
}


