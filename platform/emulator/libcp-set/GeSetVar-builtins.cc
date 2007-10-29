/*
 *  Main authors:
 *     Gustavo Gutierrez <ggutierrez@cic.puj.edu.co>
 *     Alberto Delgado <adelgado@cic.puj.edu.co>
 *
 *  Contributing authors:
 *     Alejandro Arbelaez <aarbelaez@puj.edu.co>
 *
 *  Copyright:
 *     Gustavo Gutierrez, 2006
 *     Alberto Delgado, 2006
 *
 *  Last modified:
 *     $Date$
 *     $Revision$
 *
 *  This file is part of GeOz, a module for integrating gecode 
 *  constraint system to Mozart: 
 *     http://home.gna.org/geoz
 *
 *  See the file "LICENSE" for information on usage and
 *  redistribution of this file, and for a
 *     DISCLAIMER OF ALL WARRANTIES.
 *
 */


#ifndef __GEOZ_SET_VAR_BUILTINS_CC__
#define __GEOZ_SET_VAR_BUILTINS_CC__

#include "GeSetVar-builtins.hh"


using namespace Gecode;
using namespace Gecode::Set;

/** 
 * \brief Creates a new SetVar variable with bounds
 * 
 * @param 0 The space
 * @param 1 Domain description
 * @param 2 The new variable
 */
OZ_BI_define(new_bounds,2,1)
{
  DECLARE_INT_SET(dom,val,0);   // the glb of the SetVar
  DECLARE_INT_SET(dom2,val1,1);   // the lub of the SetVar
  OZ_RETURN(new_GeSetVar(dom,dom2));
}
OZ_BI_end


/** 
 * \brief Test whether \a OZ_in(0) is a GeSetVar
 * 
 */
OZ_BI_define(var_is,1,1)
{
  OZ_RETURN_BOOL(OZ_isGeSetVar(OZ_in(0)));
}
OZ_BI_end


/** 
 * \brief Returns the Max number that gecode can representate
 * 
 * @param 0 Max integer in c++
 */

OZ_BI_define(set_sup,0,1)
{
  OZ_RETURN_INT(Limits::Set::int_max);
} 
OZ_BI_end

/** 
 * \brief Returns the Min number that gecode can representate
 * 
 * @param 0 Min integer in c++
 */

OZ_BI_define(set_inf,0,1)
{
  OZ_RETURN_INT(Limits::Set::int_min);
} 
OZ_BI_end


/** 
 * \brief Returns the minimum elemen in the domain
 * 
 * @param intvar_getMin 
 * @param 0 A reference to the variable 
 * @param 1 The minimum of the domain 
 */
/*OZ_BI_define(intvar_getMin,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(v.min());
}
OZ_BI_end
*/
/** 
 * \brief Returns the maximum elemen in the domain
 * 
 * @param intvar_getMin 
 * @param 0 A reference to the variable 
 * @param 1 The maximum of the domain 
 */
/*OZ_BI_define(intvar_getMax,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(v.max());
}
OZ_BI_end
*/
/** 
 * \brief Returns the size of the domain
 * 
 * @param OZ_in(0) A reference to the variable 
 * @param OZ_out(0) The domain size
 */
/*OZ_BI_define(intvar_getSize,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(v.size());
}
OZ_BI_end
*/

/** 
 * \brief Returns the median of the domain
 * 
 * @param OZ_in(0) A reference to the variable 
 * @param OZ_out(0) The median
 */
/*OZ_BI_define(intvar_getMed,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(v.med());
}
OZ_BI_end
*/

/** 
 * \brief Returns the width of the domain
 * 
 * @param OZ_in(0) A reference to the variable 
 * @param OZ_out(0) The width
 */
/*OZ_BI_define(intvar_getWidth,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(v.width());
}
OZ_BI_end
*/
/** 
 * \brief Returns the Regret Min of the domain
 * 
 * @param OZ_in(0) A reference to the variable 
 * @param OZ_out(0) The Regret Min
 */
/*OZ_BI_define(intvar_getRegretMin,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(IntView(v).regret_min());
}
OZ_BI_end*/

/** 
 * \brief Returns the Regret Max of the domain
 * 
 * @param OZ_in(0) A reference to the variable 
 * @param OZ_out(0) The Regret Max
 */
/*OZ_BI_define(intvar_getRegretMax,1,1)
{
  DeclareGeIntVar1(0,v);
  OZ_RETURN_INT(IntView(v).regret_max());
}
OZ_BI_end*/
#endif
