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

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#else 
#error "No stdarg.h header found"
#endif /* HAVE_STDARG_H */

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
/* random variable types */
/******************************************************************************/
typedef enum {
	rv_type_generic = 0,

	rv_type_bernoulli,
	rv_type_f,
	rv_type_normal,
	rv_type_poisson,
	rv_type_rayleigh,

	/* end of random variable types */	

	RV_NR_TYPES /* has to be the last element in the enum */
} type_t;
#define NR_RANDOM_VARIABLES	RV_NR_TYPES

#define RANDVAR_DATA	data
typedef struct {
	type_t type;
#define RANDVAR_TYPE(rv)	((rv)->type)

	union {
		struct { double p; } bernoulli;
		struct { double d1, d2; } f;
		struct { double mu, sigma; } normal;
		struct { double lambda; } poisson;
		struct { double sigma; } rayleigh;
	} RANDVAR_DATA;	/* union */
} randvar_t;
#define RANDVAR_ALLOC()		ALLOC(randvar_t)

#define RV_NR_PARAMS(name, nr_params)					\
	static const int rv_ ##name ##_nr_params = nr_params;

#define CREATE_RANDVAR_ACCESSOR(name, param, type)			\
	static inline type 						\
	randvar_##name ##_ ##param(randvar_t *rv)			\
	{								\
		return rv->RANDVAR_DATA . name . param;			\
	}								\
	static inline void						\
	randvar_##name ##_set_ ##param(randvar_t *rv, type param)	\
	{								\
		rv->RANDVAR_DATA . name . param = param;		\
	}

#define CREATE_RANDVAR_OUTCOME_FUNC1(name, func, type, param)		\
	static inline type 						\
	randvar_##name ##_ ##outcome(randvar_t *rv)			\
	{								\
		return func(randvar_##name ##_ ##param(rv));		\
	}

#define CREATE_RANDVAR_OUTCOME_FUNC2(name, func, type, param1, param2)	\
	static inline type						\
	randvar_##name ##_ ##outcome(randvar_t *rv)			\
	{								\
		return func(	randvar_##name ##_ ##param1(rv), 	\
				randvar_##name ##_ ##param2(rv) ); 	\
	}

#define CREATE_RANDVAR_RB_OUTCOME(name, conv)				\
	static VALUE							\
	randvar_##name ##_rb_ ##outcome(randvar_t *rv)			\
	{								\
		return 	conv((randvar_##name ##_outcome(rv)));		\
	}								\


/* generic */
RV_NR_PARAMS(generic, 1)
/* bernoulli */
RV_NR_PARAMS(bernoulli, 1)
CREATE_RANDVAR_ACCESSOR(bernoulli, p, double)
CREATE_RANDVAR_OUTCOME_FUNC1(bernoulli, gen_bernoulli, int, p)
CREATE_RANDVAR_RB_OUTCOME(bernoulli, INT2NUM)
/* f */
RV_NR_PARAMS(f, 2)
CREATE_RANDVAR_ACCESSOR(f, d1, double)
CREATE_RANDVAR_ACCESSOR(f, d2, double)
CREATE_RANDVAR_OUTCOME_FUNC2(f, genf, double, d1, d2)
CREATE_RANDVAR_RB_OUTCOME(f, DBL2NUM)
/* normal */
RV_NR_PARAMS(normal, 2)
CREATE_RANDVAR_ACCESSOR(normal, mu, double)
CREATE_RANDVAR_ACCESSOR(normal, sigma, double)
CREATE_RANDVAR_OUTCOME_FUNC2(normal, gennor, double, mu, sigma)
CREATE_RANDVAR_RB_OUTCOME(normal, DBL2NUM)
/* poisson */
RV_NR_PARAMS(poisson, 1)
CREATE_RANDVAR_ACCESSOR(poisson,lambda, double)
CREATE_RANDVAR_OUTCOME_FUNC1(poisson, ignpoi, long, lambda)
CREATE_RANDVAR_RB_OUTCOME(poisson, LONG2NUM)
/* rayleigh */
RV_NR_PARAMS(rayleigh, 1)
CREATE_RANDVAR_ACCESSOR(rayleigh, sigma, double)
CREATE_RANDVAR_OUTCOME_FUNC1(rayleigh, gen_rayleigh, double, sigma)
CREATE_RANDVAR_RB_OUTCOME(rayleigh, DBL2NUM)

/******************************************************************************/
/* class and module objects */
/******************************************************************************/
static VALUE rb_mRandomVariable;
static VALUE rb_cRandomVariables[NR_RANDOM_VARIABLES];

/* function callbacks */
static VALUE (*outcome_func[NR_RANDOM_VARIABLES])(randvar_t *);

static type_t type(VALUE rb_obj)
{
	int i;
	for (i = 0; i < NR_RANDOM_VARIABLES; i++)
		if (rb_obj == rb_cRandomVariables[i])
			return i;

	/* it should never reach this point */
	rb_raise(rb_eException, "unkown random variable type");
}


/******************************************************************************/
/* functions and macros for parameter validity checks */
/******************************************************************************/
#define CHECK_POSITIVE(x) do {						\
				if ((x) <= 0.0)				\
					rb_raise(rb_eArgError,		\
						"non-positive " #x 	\
						" parameter");		\
			} while (0)

#define VAR_DECLARATIONS	va_list ap;				\
				randvar_t *rv = NULL;			\
				VALUE rb_rv = Qnil;			\
				VALUE klass = 0;
#define SET_KLASS(name)							\
		(klass = rb_cRandomVariables[rv_type_ ##name])
#define GET_NEXT_ARG(ap)	va_arg((ap), VALUE)
#define CREATE_WRAPPING(rv)	Data_Wrap_Struct(klass, NULL, xfree, (rv))

#define SET_PARAM(name, param)						\
	randvar_ ##name ##_set_ ##param(rv, param)

/* memory allocation !! */
#define RANDVAR_INIT(name)						\
	do {								\
		rv = RANDVAR_ALLOC();					\
		RANDVAR_TYPE(rv) = rv_type_ ##name;			\
		rb_rv = CREATE_WRAPPING(rv);				\
	} while (0)		

#define CASE(name)							\
			case rv_type_ ##name:				\
			{		

#define CASE_END							\
			break;						\
			}	
/******************************************************************************/
/* instantiate Ruby random variable objects */
/******************************************************************************/
VALUE rb_create_instance(VALUE rb_obj, ...)
{
	VAR_DECLARATIONS

	/* initialize ap for use with the va_arg and va_end macros */
	va_start(ap, rb_obj);

	switch (type(rb_obj)) {
		CASE(bernoulli)
			VALUE rb_p;
			double p;

			SET_KLASS(bernoulli);

			rb_p = GET_NEXT_ARG(ap);
			p = NUM2DBL(rb_p);
	
			/* p > 0 */
			CHECK_POSITIVE(p);

			/* p parameter correct */
			RANDVAR_INIT(bernoulli);
			SET_PARAM(bernoulli, p);
		CASE_END

		CASE(f)
			VALUE rb_d1, rb_d2;
			double d1, d2;
			
			SET_KLASS(f);

			rb_d1 = GET_NEXT_ARG(ap);
			rb_d2 = GET_NEXT_ARG(ap);

			d1 = NUM2DBL(rb_d1);
			d2 = NUM2DBL(rb_d2);

			/* d1 > 0 */
			/* d2 > 0 */
			CHECK_POSITIVE(d1);
			CHECK_POSITIVE(d2);

			/* d1, d2 parameters correct */
			RANDVAR_INIT(f);
			SET_PARAM(f, d1);
			SET_PARAM(f, d2);
		CASE_END

		CASE(normal)
			VALUE rb_mu, rb_sigma;
			double mu, sigma;
			
			SET_KLASS(normal);

			rb_mu = GET_NEXT_ARG(ap);
			rb_sigma = GET_NEXT_ARG(ap);
			mu = NUM2DBL(rb_mu);
			sigma = NUM2DBL(rb_sigma);
			
			/* sigma > 0 */
			CHECK_POSITIVE(sigma);
			
			/* sigma parameter correct */
			RANDVAR_INIT(normal);
			SET_PARAM(normal, mu);
			SET_PARAM(normal, sigma);
		CASE_END		

		CASE(poisson)
			VALUE rb_lambda;
			double lambda;
			
			SET_KLASS(poisson);

			rb_lambda = GET_NEXT_ARG(ap);
			lambda = NUM2DBL(rb_lambda); 
			/* lambda > 0 */
			CHECK_POSITIVE(lambda);
			
			/* lambda parameter correct */
			RANDVAR_INIT(poisson);
			SET_PARAM(poisson, lambda);
		CASE_END
	
		CASE(rayleigh)
			VALUE rb_sigma;
			double sigma;

			SET_KLASS(rayleigh);

			rb_sigma = GET_NEXT_ARG(ap);
			sigma = NUM2DBL(rb_sigma);
		
			/* sigma > 0 */
			CHECK_POSITIVE(sigma);

			RANDVAR_INIT(rayleigh);
			SET_PARAM(rayleigh, sigma);				
		CASE_END

		default:
			rb_rv = Qnil;
				
	} /* switch */
	va_end(ap);
	return rb_rv;
}
#undef VAR_DECLARATIONS
#undef CREATE_WRAPPING
#undef GET_NEXT_ARG
#undef SET_PARAM
#undef RANDVAR_INIT
#undef CASE
#undef CASE_END

/******************************************************************************/
/* obtain a single outcome from the Ruby random variable object */
/******************************************************************************/
#define GET_DATA(rb_obj, rv)	Data_Get_Struct((rb_obj), randvar_t, (rv))
VALUE rb_outcome(VALUE rb_obj)
{
	randvar_t *rv = NULL;
	
	GET_DATA(rb_obj, rv);

	return (*(outcome_func[RANDVAR_TYPE(rv)]))(rv);
}

static inline long get_nr_times(VALUE rb_nr_times)
{
	long nr_times;
	Check_Type(rb_nr_times, T_FIXNUM);
	if (0 > (nr_times = NUM2LONG(rb_nr_times)))
		rb_raise(rb_eArgError, 
			"the number of outcomes cannot be negative");	
	return nr_times;
}

/******************************************************************************/
/* obtain several outcomes from the Ruby random variable object */
/******************************************************************************/
VALUE rb_outcomes(VALUE rb_obj, VALUE rb_nr_times)
{
	randvar_t *rv = NULL;
	VALUE outcomes_ary;
	long nr_times;
	VALUE (*func)(randvar_t *);

	nr_times = get_nr_times(rb_nr_times);
	GET_DATA(rb_obj, rv);

	func = outcome_func[RANDVAR_TYPE(rv)];	
	for (outcomes_ary = rb_ary_new(); nr_times > 0; --nr_times) 
		rb_ary_push(outcomes_ary, (*func)(rv));
	return outcomes_ary;
}
#undef GET_DATA

/******************************************************************************/
/* extension entry point */
/******************************************************************************/
#define CREATE_RANDOM_VARIABLE_CLASS(rb_name, name)			\
	do {								\
		VALUE *rb_objp = &rb_cRandomVariables[rv_type_ ##name]; \
									\
		*rb_objp = rb_define_class_under(rb_mRandomVariable,	\
			rb_name, rb_cRandomVariables[rv_type_generic]);	\
									\
		rb_define_singleton_method(*rb_objp, "new", 		\
			(VALUE (*) (ANYARGS)) rb_create_instance,	\
			rv_ ##name ##_nr_params);			\
									\
		rb_define_method(*rb_objp, "outcome" , rb_outcome,  0);	\
		rb_define_method(*rb_objp, "outcomes", rb_outcomes, 1);	\
		outcome_func[rv_type_ ##name] = 			\
				randvar_ ##name ##_rb_outcome;		\
	} while (0)


void Init_random_variable(void)
{
	/* the RandomVariable module */
	rb_mRandomVariable = rb_define_module("RandomVariable");
	/* Generic */
	rb_cRandomVariables[rv_type_generic] = 
		rb_define_class_under(rb_mRandomVariable, 
						"Generic", rb_cObject);
	CREATE_RANDOM_VARIABLE_CLASS("Bernoulli", bernoulli);
	CREATE_RANDOM_VARIABLE_CLASS("F", f);
	CREATE_RANDOM_VARIABLE_CLASS("Normal", normal);
	CREATE_RANDOM_VARIABLE_CLASS("Poisson", poisson);
	CREATE_RANDOM_VARIABLE_CLASS("Rayleigh", rayleigh);
}
#undef CREATE_RANDOM_VARIABLE_CLASS

/* old code */
#if 0
/******************************************************************************/
/* class objects */
/******************************************************************************/
static VALUE rb_mRandomVariable;
static VALUE rb_cGenericRV;
static VALUE rb_cBernoulliRV;
static VALUE rb_cBinomialRV;
static VALUE rb_cPoissonRV;
static VALUE rb_cNormalRV;
static VALUE rb_cExponentialRV;
static VALUE rb_cRayleighRV;
static VALUE rb_cContinuousUniformRV;
static VALUE rb_cDiscreteUniformRV;
static VALUE rb_cBetaRV;
static VALUE rb_cFRV;
/******************************************************************************/
/* Common Functions */
/******************************************************************************/
#define CAST(f) ((VALUE (*)(void *))(f))

static inline void check_not_nan(double x, const char *str)
{
	if (isnan(x))
		rb_raise(rb_eArgError, "%s", str);
}

#define CHECK_NOT_NAN(x) check_not_nan((x), \
			"not a number (NaN) " #x " parameter")

static inline void check_not_infinite(double x, const char *str)
{
	if (isfinite(x))
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define CHECK_NOT_INFINITE(x) check_not_infinite((x),"infinite " #x " paramter")


static inline void dbl_check_positive(double x, const char *str)
{
	if (x > 0.0)
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define DBL_CHECK_POSITIVE(x) dbl_check_positive((x), \
			"non-positive " #x " parameter")


static inline void long_check_positive(long x, const char *str)
{
	if (x > 0)
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define LONG_CHECK_POSITIVE(x) long_check_positive((x), \
			"non-positive " #x " parameter")

static inline void dbl_check_non_neg(double x, const char *str)
{
	if (x >= 0.0)
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define DBL_CHECK_NON_NEG(x) long_check_positive((x), \
			"negative " #x " parameter")

static inline void long_check_non_neg(long x, const char *str)
{
	if (x >= 0)
		return;
	rb_raise(rb_eArgError, "%s", str);
}

#define LONG_CHECK_NON_NEG(x) long_check_non_neg((x), \
			"negative " #x " parameter")

static inline int
dbl_check_cinterval(double x, double min, double max, 
			const char *lower, const char *greater)
{
	if (x < min)
		rb_raise(rb_eArgError, "%s", lower);
	else if (x > max)			
		rb_raise(rb_eArgError, "%s", greater);
	return;
}

#define DBL_CHECK_CINTERVAL(x,min,max)				\
	(dbl_check_cinterval((x), (min), (max), 		\
	#x " parameter out of bounds (" #x " < " #min ")", 	\
	#x " parameter out of bounds (" #x " > " #max ")")) 	

#define CHECK_PROBABILITY(x) DBL_CHECK_CINTERVAL(x,0.0,1.0)

static inline int
dbl_check_ointerval(double x, double min, double max, 
			const char *lower, const char *greater)
{
	if (x <= min)
		rb_raise(rb_eArgError, "%s", lower);
	else if (x >= max)			
		rb_raise(rb_eArgError, "%s", greater);
	return;
}

#define DBL_CHECK_OINTERVAL(x,min,max)				\
	(dbl_check_ointerval((x), (min), (max), 		\
	#x " parameter out of bounds (" #x " <= " #min ")", 	\
	#x " parameter out of bounds (" #x " >= " #max ")")) 	

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
/* Binomial Random Variable */
/******************************************************************************/
typedef struct {
	long n;
	double p;
} binomial_rv_t;

binomial_rv_t *binomial_rv_create(long n, double p)
{
	binomial_rv_t *rv;

	LONG_CHECK_NON_NEG(n);
	

	CHECK_NOT_NAN(p);
	CHECK_NOT_INFINITE(p);
	CHECK_PROBABILITY(p);

	/* n & p parameters correct */	
	rv = ALLOC(binomial_rv_t);
	rv->n = n;
	rv->p = p;
	return rv;
}

VALUE BinomialRV_new(VALUE self, VALUE n, VALUE p)
{
	binomial_rv_t *rv;
	VALUE rb_obj; 

	Check_Type(n, T_FIXNUM);
	rv = binomial_rv_create(NUM2LONG(n), NUM2DBL(p));
	rb_obj = Data_Wrap_Struct(rb_cBinomialRV, NULL, xfree, rv);
	return rb_obj;	
}

static inline binomial_rv_t *_BinomialRV(VALUE BinomialRV_obj)
{
	binomial_rv_t *rv;
	Data_Get_Struct(BinomialRV_obj, binomial_rv_t, rv);
	return rv;
}

VALUE BinomialRV_n(VALUE self)
{
	return LONG2NUM(_BinomialRV(self)->n);
}

VALUE BinomialRV_p(VALUE self)
{
	return rb_float_new(_BinomialRV(self)->p);
}

static inline VALUE _BinomialRV_outcome(binomial_rv_t *rv)
{
	return INT2FIX(ignbin(rv->n, rv->p));
}

VALUE BinomialRV_outcome(VALUE self)
{
	return _BinomialRV_outcome(_BinomialRV(self));	
}

VALUE BinomialRV_outcomes(VALUE self, VALUE times)
{
	binomial_rv_t *rv;
	
	rv = _BinomialRV(self);
	return _RV_outcomes(rv, CAST(_BinomialRV_outcome), times);
}	

/******************************************************************************/
/* Poisson Random Variable */
/******************************************************************************/
typedef struct {
	double lambda;
} poisson_rv_t;

/* avoid overflow */
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

	check_not_nan(a, "not a number (NaN) a parameter");
	check_not_infinite(a, "infinite a parameter");

	check_not_nan(b, "not a number (NaN) b parameter");
	check_not_infinite(b, "infinite b parameter");

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

	if (a >= b) {
		rb_raise(rb_eArgError, "not a < b");
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
	return LONG2NUM(_DiscreteUniformRV(self)->a);
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
/* Beta Random Variable */
/******************************************************************************/
typedef struct {
	double alpha;
	double beta;
} beta_rv_t;

static inline
beta_rv_t *beta_rv_create(double alpha, double beta)
{
	beta_rv_t *rv;

	CHECK_NOT_NAN(alpha);
	CHECK_NOT_INFINITE(alpha);

	CHECK_NOT_NAN(beta);
	CHECK_NOT_INFINITE(beta);

	DBL_CHECK_POSITIVE(alpha);
	DBL_CHECK_POSITIVE(beta);

	rv = ALLOC(beta_rv_t);
	
	rv->alpha = alpha;
	rv->beta = beta;
	return rv;
}

VALUE BetaRV_new(VALUE self, VALUE alpha, VALUE beta)
{
	beta_rv_t *rv;
	VALUE BetaRV_obj;

	rv = beta_rv_create(NUM2DBL(alpha), NUM2DBL(beta));
	BetaRV_obj = 
		Data_Wrap_Struct(rb_cBetaRV, NULL, xfree, rv);
	return BetaRV_obj;	
}

static inline 
beta_rv_t *_BetaRV(VALUE BetaRV_obj)
{
	beta_rv_t *rv;
	Data_Get_Struct(BetaRV_obj, beta_rv_t, rv);
	return rv;
}

VALUE BetaRV_alpha(VALUE self)
{
	return DBL2NUM(_BetaRV(self)->alpha);
}

VALUE BetaRV_beta(VALUE self)
{
	return DBL2NUM(_BetaRV(self)->beta);
}

static inline VALUE _BetaRV_outcome(beta_rv_t *rv)
{
	return rb_float_new(genbet(rv->alpha, rv->beta));
}

VALUE BetaRV_outcome(VALUE self)
{
	return _BetaRV_outcome(_BetaRV(self));
}

VALUE BetaRV_outcomes(VALUE self, VALUE times)
{
	beta_rv_t *rv;
	
	rv = _BetaRV(self);
	return _RV_outcomes(rv, CAST(_BetaRV_outcome), times);
}

/******************************************************************************/
/* F Random Variable */
/******************************************************************************/
typedef struct {
	double d1;
	double d2;
} f_rv_t;

static inline
f_rv_t *f_rv_create(double d1, double d2)
{
	f_rv_t *rv;

	CHECK_NOT_NAN(d1);
	CHECK_NOT_INFINITE(d1);

	CHECK_NOT_NAN(d2);
	CHECK_NOT_INFINITE(d2);

	DBL_CHECK_POSITIVE(d1);
	DBL_CHECK_POSITIVE(d2);

	rv = ALLOC(f_rv_t);
	
	rv->d1 = d1;
	rv->d2 = d2;
	return rv;
}

VALUE FRV_new(VALUE self, VALUE d1, VALUE d2)
{
	f_rv_t *rv;
	VALUE FRV_obj;

	rv = f_rv_create(NUM2DBL(d1), NUM2DBL(d2));
	FRV_obj = 
		Data_Wrap_Struct(rb_cFRV, NULL, xfree, rv);
	return FRV_obj;	
}

static inline 
f_rv_t *_FRV(VALUE FRV_obj)
{
	f_rv_t *rv;
	Data_Get_Struct(FRV_obj, f_rv_t, rv);
	return rv;
}

VALUE FRV_d1(VALUE self)
{
	return DBL2NUM(_FRV(self)->d1);
}

VALUE FRV_d2(VALUE self)
{
	return DBL2NUM(_FRV(self)->d2);
}

static inline VALUE _FRV_outcome(f_rv_t *rv)
{
	return rb_float_new(genf(rv->d1, rv->d2));
}

VALUE FRV_outcome(VALUE self)
{
	return _FRV_outcome(_FRV(self));
}

VALUE FRV_outcomes(VALUE self, VALUE times)
{
	f_rv_t *rv;
	
	rv = _FRV(self);
	return _RV_outcomes(rv, CAST(_FRV_outcome), times);
}

/******************************************************************************/
/* Extension Initialization */
/******************************************************************************/
void Init_random_variable(void)
{
	/* RandomVariable */
	rb_mRandomVariable = rb_define_module("RandomVariable");

	/* Generic */
	rb_cGenericRV = rb_define_class_under(rb_mRandomVariable, "Generic",
						rb_cObject);

	/* Bernoulli */
	rb_cBernoulliRV = rb_define_class_under(rb_mRandomVariable,"Bernoulli", rb_cGenericRV);
	rb_define_singleton_method(rb_cBernoulliRV, "new", BernoulliRV_new, 1);
	rb_define_method(rb_cBernoulliRV, "p", BernoulliRV_p, 0);
	rb_define_method(rb_cBernoulliRV, "outcome", BernoulliRV_outcome, 0);
	rb_define_method(rb_cBernoulliRV, "outcomes", BernoulliRV_outcomes, 1);

	/* Binomial */
	rb_cBinomialRV = rb_define_class_under(rb_mRandomVariable,"Binomial", rb_cGenericRV);
	rb_define_singleton_method(rb_cBinomialRV, "new", BinomialRV_new, 2);
	rb_define_method(rb_cBinomialRV, "n", BinomialRV_n, 0);
	rb_define_method(rb_cBinomialRV, "p", BinomialRV_p, 0);
	rb_define_method(rb_cBinomialRV, "outcome", BinomialRV_outcome, 0);
	rb_define_method(rb_cBinomialRV, "outcomes", BinomialRV_outcomes, 1);

	/* Poisson */
	rb_cPoissonRV = rb_define_class_under(rb_mRandomVariable,"Poisson", rb_cGenericRV);
	rb_define_singleton_method(rb_cPoissonRV, "new", PoissonRV_new, 1);
	rb_define_singleton_method(rb_cPoissonRV, 
			"lambda_max", PoissonRV_max_lambda, 0);
	rb_define_method(rb_cPoissonRV, "lambda", PoissonRV_lambda, 0);	
	rb_define_method(rb_cPoissonRV, "mean", PoissonRV_lambda, 0);	
	rb_define_method(rb_cPoissonRV, "var", PoissonRV_lambda, 0);	
	rb_define_method(rb_cPoissonRV, "outcome", PoissonRV_outcome, 0);	
	rb_define_method(rb_cPoissonRV, "outcomes", PoissonRV_outcomes, 1);

	/* Normal */
	rb_cNormalRV = rb_define_class_under(rb_mRandomVariable,"Normal", rb_cGenericRV);
	rb_define_singleton_method(rb_cNormalRV, "new", NormalRV_new, 2);
	rb_define_method(rb_cNormalRV, "outcome", NormalRV_outcome, 0);	
	rb_define_method(rb_cNormalRV, "outcomes", NormalRV_outcomes, 1);

	/* Exponential */
	rb_cExponentialRV = rb_define_class_under(rb_mRandomVariable,"Exponential", rb_cGenericRV);
	rb_define_singleton_method(rb_cExponentialRV, "new", 
						ExponentialRV_new, 1);
	rb_define_method(rb_cExponentialRV, "lambda", ExponentialRV_lambda, 0);
	rb_define_method(rb_cExponentialRV, "rate", ExponentialRV_lambda, 0);
	rb_define_method(rb_cExponentialRV, "outcome", 
						ExponentialRV_outcome, 0);
	rb_define_method(rb_cExponentialRV, "outcomes", 
						ExponentialRV_outcomes, 1);

	/* Rayleigh */
	rb_cRayleighRV = rb_define_class_under(rb_mRandomVariable,"Rayleigh", rb_cGenericRV);
	rb_define_singleton_method(rb_cRayleighRV, "new",
						RayleighRV_new, 1);
	rb_define_method(rb_cRayleighRV, "sigma", RayleighRV_sigma, 0);
	rb_define_method(rb_cRayleighRV, "outcome", 
						RayleighRV_outcome, 0);
	rb_define_method(rb_cRayleighRV, "outcomes", 
						RayleighRV_outcomes, 1);

	/* Continuous Uniform */
	rb_cContinuousUniformRV = 
		rb_define_class_under(rb_mRandomVariable,"ContinuousUniform", rb_cGenericRV);
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
		rb_define_class_under(rb_mRandomVariable,"DiscreteUniform", rb_cGenericRV);
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

	/* Beta */
	rb_cBetaRV = 
		rb_define_class_under(rb_mRandomVariable,"Beta", rb_cGenericRV);
	rb_define_singleton_method(rb_cBetaRV, "new",
				BetaRV_new, 2);
	rb_define_method(rb_cBetaRV, 
				"alpha", BetaRV_alpha, 0);
	rb_define_method(rb_cBetaRV, 
				"beta", BetaRV_beta, 0);
	rb_define_method(rb_cBetaRV, "outcome", 
				BetaRV_outcome, 0);
	rb_define_method(rb_cBetaRV, "outcomes", 
				BetaRV_outcomes, 1);

	/* F */
	rb_cFRV = 
		rb_define_class_under(rb_mRandomVariable,"F", rb_cGenericRV);
	rb_define_singleton_method(rb_cFRV, "new",
				FRV_new, 2);
	rb_define_method(rb_cFRV, 
				"d1", FRV_d1, 0);
	rb_define_method(rb_cFRV, 
				"d2", FRV_d2, 0);
	rb_define_method(rb_cFRV, "outcome", 
				FRV_outcome, 0);
	rb_define_method(rb_cFRV, "outcomes", 
				FRV_outcomes, 1);
	
	return;
}

#endif /* #if 0 */
