/*
 *  Authors:
 *    Michael Mehl (mehl@dfki.de)
 * 
 *  Contributors:
 *    Raphael Collet (raph@info.ucl.ac.be)
 *    Alfred Spiessens (fsp@info.ucl.ac.be)
 * 
 *  Copyright:
 *    Michael Mehl (1998)
 * 
 *  Last change:
 *    $Date$ by $Author$
 *    $Revision$
 * 
 *  This file is part of Mozart, an implementation 
 *  of Oz 3:
 *     http://www.mozart-oz.org
 * 
 *  See the file "LICENSE" or
 *     http://www.mozart-oz.org/LICENSE.html
 *  for information on usage and redistribution 
 *  of this file, and for a DISCLAIMER OF ALL 
 *  WARRANTIES.
 *
 */

#if defined(INTERFACE) && !defined(VAR_ALL)
#pragma implementation "var_simple.hh"
#endif

#include "var_simple.hh"
#include "unify.hh"

OZ_Return SimpleVar::bind(TaggedRef* vPtr, TaggedRef t)
{
  oz_bindVar(this, vPtr, t);
  return PROCEED;
}

// getType(vPtr) == OZ_VAR_SIMPLE
// getBoard(lvp) != getBoard(rvp) || getType(vPtr) >= getType(tPtr)
// in particular (*tPtr) can not be OZ_VAR_OPT [when in the same space]
OZ_Return SimpleVar::unify(TaggedRef* vPtr, TaggedRef *tPtr)
{
//    OzVariable *tv=tagged2Var(*tPtr);
//    // kost@ : OZ_VAR_OPT"s are yet simpler than OZ_VAR_SIMPLE"s;
//    if (tv->getType() >= OZ_VAR_SIMPLE
//        && oz_isBelow(tv->getBoardInternal(), GETBOARD(this))
//  #ifdef VAR_BIND_NEWER
//        // if both are local, then check heap
//        && (!oz_isLocalVar(this) || heapNewer(tPtr,vPtr))
//  #endif
//        ) {
//      oz_bindVar(tv,tPtr, makeTaggedRef(vPtr));
//    } else {
  oz_bindVar(this,vPtr, makeTaggedRef(tPtr));
//    }
  return (PROCEED);
}

// this method forces the variable to become needed; it kicks the
// suspension list and change the variable's type.
OZ_Return SimpleVar::becomeNeeded()
{
  // release the current suspension list
  oz_checkSuspensionList(this, pc_all);
  // mutate into a needed variable
  setType(OZ_VAR_SIMPLE);
  // disposeS();
  return PROCEED;
}

// use this method for adding demanding suspensions only!
OZ_Return SimpleVar::addSusp(TaggedRef *tPtr, Suspendable * susp) {
  // release the current suspension list and mutate into a needed var
  becomeNeeded();
  // add susp into the SimpleVar's suspension list
  addSuspSVar(susp);
  return SUSPEND;
}

OzVariable *oz_newSimpleVar(Board *bb)
{
  return new SimpleVar(bb);
}
