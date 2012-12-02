////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// File:     random_variable.c                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Author:   Jorge F.M. Rinaldi                                               //
// Contact:  jorge.madronal.rinaldi@gmail.com                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Date:     2012/10/11                                                       //
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

#include "gen.h"
#include "randlib.h"
#include "xrandlib.h"

/******************************************************************************/
/* random variable types */
/******************************************************************************/
typedef enum {
	rv_type_generic = 0,

	rv_type_bernoulli,
	rv_type_beta,
	rv_type_binomial,
	rv_type_chi_squared,
	rv_type_continuous_uniform,
	rv_type_discrete_uniform,
	rv_type_exponential,
	rv_type_f,
	rv_type_normal,
	rv_type_poisson,
	rv_type_rayleigh,

	/* end of random variable types */	

	RV_NR_TYPES /* has to be the last element in the enum */
} type_t;
#define NR_RANDOM_VARIABLES	RV_NR_TYPES
/******************************************************************************/

#define RANDVAR_DATA	data
typedef struct {
	type_t type;
#define RANDVAR_TYPE(rv)	((rv)->type)

	union {
		struct { double p; } bernoulli;
		struct { double alpha, beta; } beta;
		struct { long n; double p; } binomial;
		struct { long k; } chi_squared;
		struct { double a,b; } continuous_uniform;
		struct { long a,b; } discrete_uniform;
		struct { double mean; } exponential;
		struct { double d1, d2; } f;
		struct { double mu, sigma; } normal;
		struct { double mean; } poisson;
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
/* beta */
RV_NR_PARAMS(beta, 2)
CREATE_RANDVAR_ACCESSOR(beta, alpha, double)
CREATE_RANDVAR_ACCESSOR(beta, beta, double)
CREATE_RANDVAR_OUTCOME_FUNC2(beta, genbet, double, alpha, beta)
CREATE_RANDVAR_RB_OUTCOME(beta, DBL2NUM)
/* binomial */
RV_NR_PARAMS(binomial, 2)
CREATE_RANDVAR_ACCESSOR(binomial, n, long)
CREATE_RANDVAR_ACCESSOR(binomial, p, double)
CREATE_RANDVAR_OUTCOME_FUNC2(binomial, ignbin, long, n, p)
CREATE_RANDVAR_RB_OUTCOME(binomial, LONG2NUM)
/* chi-squared */
RV_NR_PARAMS(chi_squared, 1)
CREATE_RANDVAR_ACCESSOR(chi_squared, k, long)
CREATE_RANDVAR_OUTCOME_FUNC1(chi_squared, gen_chi_squared, double, k)
CREATE_RANDVAR_RB_OUTCOME(chi_squared, DBL2NUM)
/* continuous uniform */
RV_NR_PARAMS(continuous_uniform, 2)
CREATE_RANDVAR_ACCESSOR(continuous_uniform, a, double)
CREATE_RANDVAR_ACCESSOR(continuous_uniform, b, double)
CREATE_RANDVAR_OUTCOME_FUNC2(continuous_uniform, genunf, double, a, b)
CREATE_RANDVAR_RB_OUTCOME(continuous_uniform, DBL2NUM)
/* discrete uniform */
RV_NR_PARAMS(discrete_uniform, 2)
CREATE_RANDVAR_ACCESSOR(discrete_uniform, a, long)
CREATE_RANDVAR_ACCESSOR(discrete_uniform, b, long)
CREATE_RANDVAR_OUTCOME_FUNC2(discrete_uniform, ignuin, long, a, b)
CREATE_RANDVAR_RB_OUTCOME(discrete_uniform, LONG2NUM)
/* exponential */
RV_NR_PARAMS(exponential, 1)
CREATE_RANDVAR_ACCESSOR(exponential, mean, double)
CREATE_RANDVAR_OUTCOME_FUNC1(exponential, genexp , double, mean)
CREATE_RANDVAR_RB_OUTCOME(exponential, DBL2NUM)
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
CREATE_RANDVAR_ACCESSOR(poisson, mean, double)
CREATE_RANDVAR_OUTCOME_FUNC1(poisson, ignpoi, long, mean)
CREATE_RANDVAR_RB_OUTCOME(poisson, LONG2NUM)
/* rayleigh */
RV_NR_PARAMS(rayleigh, 1)
CREATE_RANDVAR_ACCESSOR(rayleigh, sigma, double)
CREATE_RANDVAR_OUTCOME_FUNC1(rayleigh, gen_rayleigh, double, sigma)
CREATE_RANDVAR_RB_OUTCOME(rayleigh, DBL2NUM)

/******************************************************************************/
/* class and module objects */
static VALUE rb_mRandomVariable;
static VALUE rb_mGenerator;
static VALUE rb_cRandomVariables[NR_RANDOM_VARIABLES];
/******************************************************************************/

/******************************************************************************/
/* function callbacks for the random number generating function */
static VALUE (*outcome_func[NR_RANDOM_VARIABLES])(randvar_t *);
/******************************************************************************/

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
#define CHECK_POSITIVE(x) 						\
			do {						\
				if ((x) <= 0.0)				\
					rb_raise(rb_eArgError,		\
						"non-positive " #x 	\
						" parameter");		\
			} while (0)

#define CHECK_NON_NEGATIVE(x) 						\
			do {						\
				if ((x) < 0.0)				\
					rb_raise(rb_eArgError,		\
						"negative " #x 		\
						" parameter");		\
			} while (0)

#define CHECK_LESS_THAN(a,b) 						\
			do {						\
				if ((a) >= (b))				\
					rb_raise(rb_eArgError,		\
						#a " parameter not "	\
						"lower than " #b	\
						" parameter");		\
			} while (0)				

#define CHECK_RB_INTEGER(x, str) 					\
			do {						\
				if (!rb_obj_is_kind_of(x, rb_cInteger))	\
					rb_raise(rb_eArgError,		\
						str " parameter not "	\
						"integer");		\
			} while (0)

#define CHECK_PROBABILITY(x) 						\
			do {						\
				if (x > 1.0 || x < 0.0)			\
					rb_raise(rb_eArgError,		\
						#x " parameter "	\
						"is not a "		\
						"probability");		\
			} while (0)	

#define VAR_DECLARATIONS	va_list ap;				\
				randvar_t *rv = NULL;			\
				VALUE rb_rv = Qnil;			\
				VALUE klass = 0;
#define SET_KLASS(name)							\
		(klass = rb_cRandomVariables[rv_type_ ##name])

#define ASSERT_KLASS_IS_SET						\
		do {							\
			if (0 == klass) 				\
				rb_bug("forgot to set klass variable "	\
					"in %s()", __func__);		\
		} while (0)						


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
			ASSERT_KLASS_IS_SET;				\
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
	
			/* 0 <= p <= 1 */
			CHECK_PROBABILITY(p);

			/* p parameter correct */
			RANDVAR_INIT(bernoulli);
			SET_PARAM(bernoulli, p);
		CASE_END

		CASE(beta)
			VALUE rb_alpha, rb_beta;
			double alpha, beta;

			SET_KLASS(beta);

			rb_alpha = GET_NEXT_ARG(ap);
			rb_beta  = GET_NEXT_ARG(ap);
			
			alpha = NUM2DBL(rb_alpha);
			beta  = NUM2DBL(rb_beta);

			/* alpha > 0 */
			CHECK_POSITIVE(alpha);

			/* beta > 0 */
			CHECK_POSITIVE(beta);

			/* alpha and beta parameters correct */
			RANDVAR_INIT(beta);
			SET_PARAM(beta, alpha);
			SET_PARAM(beta, beta);
		CASE_END

		CASE(binomial)
			VALUE rb_n, rb_p;
			long n;
			double p;

			SET_KLASS(binomial);
			
			rb_n = GET_NEXT_ARG(ap);
			rb_p = GET_NEXT_ARG(ap);

			CHECK_RB_INTEGER(rb_n, "n");

			n = NUM2LONG(rb_n);
			p = NUM2DBL(rb_p);
		
			/* n >= 0 */	
			CHECK_NON_NEGATIVE(n);

			/* 0 <= p <= 1 */
			CHECK_PROBABILITY(p);
		
			/* n and p parameters correct */	
			RANDVAR_INIT(binomial);
			SET_PARAM(binomial, n);
			SET_PARAM(binomial, p);
		CASE_END

		CASE(chi_squared)
			VALUE rb_k;
			long k;

			SET_KLASS(chi_squared);

			rb_k = GET_NEXT_ARG(ap);
			CHECK_RB_INTEGER(rb_k, "k");

			k = NUM2LONG(rb_k);
			
			/* k > 0 */
			CHECK_POSITIVE(k);

			/* k parameter correct */
			RANDVAR_INIT(chi_squared);
			SET_PARAM(chi_squared, k);	
		CASE_END

		CASE(continuous_uniform)
			VALUE rb_a, rb_b;
			double a,b;

			SET_KLASS(continuous_uniform);

			rb_a = GET_NEXT_ARG(ap);
			rb_b = GET_NEXT_ARG(ap);

			a = NUM2DBL(rb_a);
			b = NUM2DBL(rb_b);

			/* a < b */
			CHECK_LESS_THAN(a,b);

			/* a and b parameters correct */
			RANDVAR_INIT(continuous_uniform);
			SET_PARAM(continuous_uniform, a);
			SET_PARAM(continuous_uniform, b);
		CASE_END

		CASE(discrete_uniform)
			VALUE rb_a, rb_b;
			double a,b;

			SET_KLASS(discrete_uniform);

			rb_a = GET_NEXT_ARG(ap);
			rb_b = GET_NEXT_ARG(ap);

			CHECK_RB_INTEGER(rb_a, "a");
			CHECK_RB_INTEGER(rb_b, "b");

			a = NUM2LONG(rb_a);
			b = NUM2LONG(rb_b);

			/* a < b */
			CHECK_LESS_THAN(a,b);

			/* a and b parameters correct */
			RANDVAR_INIT(discrete_uniform);
			SET_PARAM(discrete_uniform, a);
			SET_PARAM(discrete_uniform, b);
		CASE_END

		CASE(exponential)
			VALUE rb_mean;
			double mean;

			SET_KLASS(exponential);

			rb_mean = GET_NEXT_ARG(ap);
			mean = NUM2DBL(rb_mean);

			/* mean > 0 */
			CHECK_POSITIVE(mean);

			/* mean parameter correct */
			RANDVAR_INIT(exponential);
			SET_PARAM(exponential, mean);
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
			VALUE rb_mean;
			double mean;
			
			SET_KLASS(poisson);

			rb_mean = GET_NEXT_ARG(ap);
			mean = NUM2DBL(rb_mean); 
			/* mean > 0 */
			CHECK_POSITIVE(mean);;
			
			/* mean parameter correct */
			RANDVAR_INIT(poisson);
			SET_PARAM(poisson, mean);;
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
#undef CHECK_POSITIVE
#undef CHECK_NON_NEGATIVE
#undef CHECK_LESS_THAN
#undef CHECK_RB_INTEGER
#undef CHECK_PROBABILITY
#undef VAR_DECLARATIONS
#undef SET_KLASS
#undef GET_NEXT_ARG
#undef CREATE_WRAPPING
#undef SET_PARAM
#undef RANDVAR_INIT
#undef CASE
#undef CASE_END


#define GET_DATA(rb_obj, rv)	Data_Get_Struct((rb_obj), randvar_t, (rv))
/******************************************************************************/
/* obtain a single outcome from the Ruby random variable object */
/******************************************************************************/
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

static VALUE rb_seed_get(VALUE self)
{
	return rv_gen_get_seed();
}

static VALUE rb_seed_set(VALUE self, VALUE rb_seed)
{
	rv_gen_set_seed(rb_seed);
	return rb_seed;
}

/******************************************************************************/
/* macros for the extension entry point */
/******************************************************************************/
#define CREATE_RANDOM_VARIABLE_CLASS(rb_name, name)			\
	do {								\
		VALUE *rb_objp;						\
		VALUE rb_metaclass;					\
									\
		rb_objp = &rb_cRandomVariables[rv_type_ ##name]; 	\
									\
		*rb_objp = rb_define_class_under(rb_mRandomVariable,	\
			rb_name, rb_cRandomVariables[rv_type_generic]);	\
									\
		rb_metaclass = rb_singleton_class(*rb_objp);		\
									\
		rb_define_private_method(rb_metaclass, "intern_new", 	\
			(VALUE (*) (ANYARGS)) rb_create_instance,	\
			rv_ ##name ##_nr_params);			\
									\
		rb_define_method(*rb_objp, "outcome" , rb_outcome,  0);	\
		rb_define_alias(*rb_objp, "sample", "outcome");		\
									\
		rb_define_method(*rb_objp, "outcomes", rb_outcomes, 1);	\
		rb_define_alias(*rb_objp, "samples", "outcomes");	\
		outcome_func[rv_type_ ##name] = 			\
				randvar_ ##name ##_rb_outcome;		\
	} while (0)

/******************************************************************************/
/* extension entry point */
/******************************************************************************/
void Init_random_variable(void)
{
	/* the RandomVariable module */
	rb_mRandomVariable = rb_define_module("RandomVariable");

	/* Generator */
	rb_mGenerator = rb_define_module_under(rb_mRandomVariable, "Generator");
	rb_define_singleton_method(rb_mGenerator, "seed", rb_seed_get, 0);
	rb_define_singleton_method(rb_mGenerator, "seed=", rb_seed_set, 1);

	/* Generic */
	rb_cRandomVariables[rv_type_generic] = 
		rb_define_class_under(rb_mRandomVariable, 
						"Generic", rb_cObject);
	/* random distributions */
	CREATE_RANDOM_VARIABLE_CLASS("Bernoulli", bernoulli);
	CREATE_RANDOM_VARIABLE_CLASS("Beta", beta);
	CREATE_RANDOM_VARIABLE_CLASS("Binomial", binomial);
	CREATE_RANDOM_VARIABLE_CLASS("ChiSquared", chi_squared);
	CREATE_RANDOM_VARIABLE_CLASS("ContinuousUniform", continuous_uniform);
	CREATE_RANDOM_VARIABLE_CLASS("DiscreteUniform", discrete_uniform);
	CREATE_RANDOM_VARIABLE_CLASS("Exponential", exponential);
	CREATE_RANDOM_VARIABLE_CLASS("F", f);
	CREATE_RANDOM_VARIABLE_CLASS("Normal", normal);
	CREATE_RANDOM_VARIABLE_CLASS("Poisson", poisson);
	CREATE_RANDOM_VARIABLE_CLASS("Rayleigh", rayleigh);

	/* initialize the random number generator */
	rv_init_gen();
}
#undef CREATE_RANDOM_VARIABLE_CLASS

