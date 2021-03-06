/*
 *  Authors:
 *    Tobias Mueller (tmueller@ps.uni-sb.de)
 * 
 *  Contributors:
 *    optional, Contributor's name (Contributor's email address)
 * 
 *  Copyright:
 *    Organization or Person (Year(s))
 * 
 *  Last change:
 *    $Date$ by $Author$
 *    $Revision$
 * 
 *  This file is part of Mozart, an implementation 
 *  of Oz 3:
 *     $MOZARTURL$
 * 
 *  See the file "LICENSE" or
 *     $LICENSEURL$
 *  for information on usage and redistribution 
 *  of this file, and for a DISCLAIMER OF ALL 
 *  WARRANTIES.
 *
 */

#ifndef __AUX_HH__
#define __AUX_HH__

#include "mozart_cpi.hh"
#include <math.h>
#include <values.h>

//-----------------------------------------------------------------------------

double * getDoubleVector(OZ_Term, double *);

#define DECLARE_FLOAT(ARG, VAR)			\
 double VAR;					\
 if (OZ_isVariable(OZ_args[ARG])) {		\
   OZ_suspendOn(OZ_args[ARG]);			\
 }						\
 if (! OZ_isFloat(OZ_args[ARG])) {		\
   return OZ_typeError(ARG,"Float");		\
   return FAILED;				\
 } else {					\
   VAR = OZ_floatToC(OZ_args[ARG]);		\
 }

#define RUNTIME_ERROR(ARGS) 			\
printf ARGS; 					\
printf("\nPropagator failed.\n"); 		\
fflush(stdout); 				\
return OZ_FAILED

//-----------------------------------------------------------------------------

#ifdef DEBUG_CHECK
#define RI_DEBUG_PRINT(ARGS) printf ARGS; fflush(stdout);
#define RI_DEBUG_PRINT_THIS(STR)			\
   RI_DEBUG_PRINT(("%s%s\n", STR, this->toString()))
#define ASSERT(Cond)					\
  if (! (Cond)) {					\
    fprintf(stderr,"%s:%d ",__FILE__,__LINE__);		\
    fprintf(stderr, " assertion '%s' failed", #Cond);	\
    abort();						\
  }
#else
#define ASSERT(Cond)
#define RI_DEBUG_PRINT(ARGS) 
#define RI_DEBUG_PRINT_THIS(STR) 	
#endif

#define USE_RI_DOUBLE

#ifdef USE_RI_DOUBLE

typedef double ri_float;
#define RI_FLOAT_MIN    -MAXDOUBLE
#define RI_FLOAT_MAX    MAXDOUBLE
#define RI_EPSILON      0.0001
#define RI_FLOAT_FORMAT "%g"

#else

typedef float ri_float;
#define RI_FLOAT_MIN    -MAXFLOAT
#define RI_FLOAT_MAX    MAXFLOAT
#define RI_EPSILON      0.0001
#define RI_FLOAT_FORMAT "%g"

#endif

#define EM_RI    "real interval"
#define EM_FLOAT "float"

extern 
OZ_Term atom_row, atom_opt, atom_type, atom_rhs, atom_min, atom_max,
  atom_optimal, atom_infeasible, atom_unbounded,  atom_failure, atom_le, 
  atom_ge, atom_eq, atom_oops, atom_solver, atom_mode, atom_avail, atom_config,
  atom_lpsolve, atom_cplex_primopt, atom_cplex_dualopt, atom_put, atom_get, 
  atom_quiet, atom_verbose;

//-----------------------------------------------------------------------------
// common stuff for propagators


#define FailOnInvalid(X) if((X) < 0.0) goto failure;

#ifdef FailOnInvalidTouched
#undef FailOnInvalidTouched
#endif

#define FailOnInvalidTouched(X, W, F)		\
{						\
  ri_float _w = (X);				\
  if(_w < 0.0)					\
    goto failure;				\
  if (_w < W - ri_precision) {			\
    F = 1;					\
    W = _w;					\
  }						\
}

#define FailOnEmpty(X) if((X) == 0) goto failure;

#ifndef ALLWAYS_CLOSE_CPLEX
#ifdef CPLEX
#include "cplex.h"
extern CPXENVptr CPLEX_env;
#endif
#endif

extern char * solver_name;

#endif /* __AUX_HH__ */
