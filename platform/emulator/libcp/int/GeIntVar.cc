/*
 *  Main authors:
 *     Raphael Collet <raph@info.ucl.ac.be>
 *     Gustavo Gutierrez <ggutierrez@cic.puj.edu.co>
 *     Alberto Delgado <adelgado@cic.puj.edu.co>
 *
 *  Contributing authors:
 *     
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


#include "GeIntVar.hh"
#include "unify.hh"

using namespace Gecode;
using namespace Gecode::Int;


/*
OZ_Return GeIntVar::unifyV(TaggedRef* lPtr, TaggedRef* rPtr) {
  ////  printf("unifyV GeIntVar\n");fflush(stdout);
  //  if (!OZ_isGeIntVar(*rPtr)) return OZ_suspendOnInternal(*rPtr);
  
  GeIntVar* lgeintvar = this;
  GenericSpace* space = extVar2Var(lgeintvar)->getBoardInternal()->getGenericSpace();

   
  // the real condition we need to check here is:
  //   if (oz_isSimpleVar(*rPtr) || oz_isOptVar(*rPtr))
  
  if (!OZ_isGeIntVar(*rPtr)) {
    // This is the case when right var is either a simple var or an
    // optimized var and this represents a global var.
    
    oz_bindGlobalVar2(tagged2Var(*rPtr),rPtr,makeTaggedRef(lPtr));
    goto PROP;
  }

  { 
    GeIntVar* rgeintvar = get_GeIntVar(*rPtr);
    IntVar& lintvar = lgeintvar->getIntVarInfo();
    IntVar& rintvar = rgeintvar->getIntVarInfo();

    if(am.inEqEq()) {
      if( IsEmptyInter(rintvar, lintvar) ) return FAILED;
      else {
	trail.pushBind(lPtr);
	trail.pushBind(rPtr);
	return PROCEED;
      }
    }
    
    Assert(space);
    if(oz_isLocalVar(extVar2Var(lgeintvar))){
      if(!oz_isLocalVar(extVar2Var(rgeintvar))){
	if(!(rgeintvar->intersect(makeTaggedRef(lPtr))))
	  return FAILED;
	////	printf("local, no local \n");fflush(stdout);
	oz_bindGlobalVar2(extVar2Var(rgeintvar), rPtr, makeTaggedRef(lPtr));	
      }else{
	////	printf("unifyV GeIntVar this is local\n");fflush(stdout);
	// "this" is local.  The binding cannot go upwards, so...    
	//Assert(oz_isLocalVar(extVar2Var(rgeintvar)));
	//I don't think this assert is correct,  other can be a global var
	//printf("local, local \n");fflush(stdout);
	if(!(rgeintvar->intersect(makeTaggedRef(lPtr))))
	  return FAILED;
	oz_bindLocalVar(extVar2Var(this), lPtr, makeTaggedRef(rPtr));
	eq(space, lintvar, rintvar);

	//should be in the same method in gevar
	lgeintvar->incLeftUnifyC();
	space->incForeignProps();
      }
    }else{
      // "this" is global.
      if (oz_isLocalVar(extVar2Var(rgeintvar))) {
	if(!(lgeintvar->intersect(makeTaggedRef(rPtr))))
	  return FAILED;
	//printf("global, local \n");fflush(stdout);
	oz_bindGlobalVar2(extVar2Var(lgeintvar), lPtr, makeTaggedRef(rPtr));	
      } else {
	//printf("global, global \n");fflush(stdout);
	OZ_Term lv = new_GeIntVar(Gecode::IntSet(Limits::Int::int_min,Gecode::Limits::Int::int_max));
	//TaggedRef glv_tmp = oz_deref(makeTaggedVar(extVar2Var(get_GeIntVar(lv))));
	if(!(lgeintvar->intersect(lv)))
	  return FAILED;
	if(!(rgeintvar->intersect(lv)))
	  return FAILED;	
	Assert(oz_isRef(lv));
	oz_bindGlobalVar2(extVar2Var(lgeintvar), lPtr, lv);	
	oz_bindGlobalVar2(extVar2Var(rgeintvar), rPtr, lv);	
      }
    }
  
  }
  // Unification is entailed by means of an eq propagator. After post this
  //   propagator the generic space must become unstable. The unstability is
  //   a result of posting the propagator 
    
 PROP:
  // wakeup space propagators to inmediatly update all related variables
  unsigned long alt = 0; //useless variable
  //    return (space->status(alt)== Gecode::SS_FAILED) ? FAILED: PROCEED ;
  if(space->status(alt) == Gecode::SS_FAILED) {
    extVar2Var(this)->getBoardInternal()->setFailed();
    return FAILED;
  }
  else
    return PROCEED;
}
*/

/*
  At this point vPtr must be a local variable (be careful)
*/
OZ_Return GeIntVar::bindV(TaggedRef* vPtr, TaggedRef val) {
  if (validV(val)) {

    if(am.inEqEq()) {
      GeIntVar* gvar = get_GeIntVar(*vPtr);
      IntVar& var = gvar->getIntVarInfo();
      IntView vvar(var);
      if( !vvar.in(OZ_intToC(val)) ) return FAILED;
      else {
	trail.pushBind(vPtr);
	return PROCEED;
      }
    }
    if (oz_isLocalVar(extVar2Var(this))) {
      // first bind the variable in Mozart
      oz_bindLocalVar(extVar2Var(this), vPtr, val);

      // then bind the IntVar in the GenericSpace
      GenericSpace* s =  extVar2Var(this)->getBoardInternal()->getGenericSpace();      
      int n = OZ_intToC(val);
      
      ModEvent me = IntView(getIntVarInfo()).eq(s, n);
      Assert(!me_failed(me));     // must succeed

      unsigned long alt = 0; //useless variable
      
      if (s->status(alt) == Gecode::SS_FAILED) {
	extVar2Var(this)->getBoardInternal()->setFailed();
	return FAILED;
      }
      return PROCEED;

      // wakeup space propagators to inmediatly update all related variables
      //unsigned int alt = 0; //useless variable
      //s->status(alt);
    } else {
      // global binding...
      oz_bindGlobalVar(extVar2Var(this), vPtr, val);
    }
    return PROCEED;
  }
  return OZ_FAILED;
}

Bool GeIntVar::validV(TaggedRef val) {
  // BigInts cannot be represented by normal c int type. 
  // SmallInts are just c ints so we only allow that kind of integer values
  // References:
  //   FD.sup = 134 217 726
  //   FD.inf = 0
  // In Gecode:
  //   Gecode::Limits::Int::int_max
  //   Gecode::Limits::Int::int_min
  if (OZ_isSmallInt(val)) {
    int n = OZ_intToC(val);
    if(n >= Gecode::Limits::Int::int_min &&
       n <= Gecode::Limits::Int::int_max)
      {
	//printf("GeIntVar::validV\n");fflush(stdout);
      return IntView(getIntVarInfo()).in(n);
      }
    else {
      GEOZ_DEBUG_PRINT(("Invalid integer.\n All domain ranges must be between %d and %d",Gecode::Limits::Int::int_min, Gecode::Limits::Int::int_max));
      return false;
    }
  }
  return false;
}

OZ_Term GeIntVar::statusV() {
  return OZ_mkTupleC("kinded", 1, OZ_atom("int"));
}


VarBase* GeIntVar::clone(void) {
  GenericSpace* gs = extVar2Var(this)->getBoardInternal()->getGenericSpace(true);
  Assert(gs);
  IntVar &v = getIntVarInfo();
  IntVar x;
  x.update(gs,false,v);
  return x.variable();
}


//(this) is the global variable
//x is the local variable,  the one that its domain is modified
bool GeIntVar::intersect(TaggedRef x) {
  IntVar& gv = getIntVarInfo();
  ViewRanges<IntView> gvr(gv);

  IntVar& liv = get_GeIntVar(x)->getIntVarInfo();
  IntView vw(liv);
  return (vw.inter(oz_currentBoard()->getGenericSpace(),gvr)==ME_GEN_FAILED ? false: true);
}

//(this) is the global variable
//lx is the local value
bool GeIntVar::In(TaggedRef lx) {
  IntVar gv = getIntVarInfo();
  IntView vw(gv);
  return vw.in(oz_intToC(lx));
}

TaggedRef GeIntVar::clone(TaggedRef v) {
  Assert(OZ_isGeIntVar(v));
  
  OZ_Term lv = new_GeIntVar(IntSet(Limits::Int::int_min,Limits::Int::int_max));
  get_GeIntVar(v)->intersect(lv);
  return lv;
}


bool GeIntVar::hasSameDomain(TaggedRef v) {
  Assert(OZ_isGeIntVar(v));
  IntVar v1 = get_GeIntVar(v)->getIntVarInfo();
  Gecode::Int::ViewRanges< Gecode::Int::IntView > vr1 (v1);
  Gecode::Int::ViewRanges< Gecode::Int::IntView > vr2 (getIntVarInfo());
  
  while(true) {
    if(!vr1() && !vr2()) return true;
    if(!vr1() || !vr2()) return false;
    if( (vr1.min() != vr2.min()) || (vr1.max() != vr2.max() ) ) return false;
    ++vr1; ++vr2;
  }
}


TaggedRef GeIntVar::newVar(){
  return new_GeIntVar(Gecode::IntSet(Gecode::Limits::Int::int_min,Gecode::Limits::Int::int_max));
}

/*this function checks if the intersection between v1 and v2 is empty or not*/

bool GeIntVar::IsEmptyInter(TaggedRef* var1,  TaggedRef* var2) {

GeIntVar* rgeintvar = get_GeIntVar(*var1);
GeIntVar* lgeintvar = get_GeIntVar(*var2);
Gecode::IntVar& v1 = lgeintvar->getIntVarInfo();
Gecode::IntVar& v2 = rgeintvar->getIntVarInfo();


//Gecode::Int::IntView v1, Gecode::Int::IntView v2

Gecode::Int::ViewRanges< Gecode::Int::IntView > vr1 (v1);
Gecode::Int::ViewRanges< Gecode::Int::IntView > vr2 (v2);
  
  while(true) {
    if(!vr1() || !vr2() ) return true;
    if( vr2.min() <= vr2.max() && vr2.max() <= vr1.max() && vr1.min() <= vr2.max() ) return false;
    if( vr1.min() <= vr2.max() && vr1.max() <= vr2.max() && vr2.min() <= vr1.max() ) return false;
    
    if( vr2.max() <= vr1.max() ) ++vr2;
    ++vr1;
  }
}

inline
void GeIntVar::ensureValReflection(OZ_Term ref) {
  Assert(OZ_isGeIntVar(ref));
  if (!hasValRefl) {
    GenericSpace *s = extVar2Var(this)->getBoardInternal()->getGenericSpace(true);
    // post the var reflector propagator
    s->setVarRef(index,ref);
    Gecode::Int::IntVarImp *ivp = reinterpret_cast<Gecode::Int::IntVarImp*>(s->getVar(index));
     GeView<Gecode::Int::IntVarImp> iv(ivp);
     Gecode::Int::IntView *vv = reinterpret_cast<Gecode::Int::IntView*>(&iv);
     new (s) IntVarReflector(s, *vv, index);
     hasValRefl = true;
  }
}

inline
void GeIntVar::ensureDomReflection(OZ_Term ref) {
  Assert(OZ_isGeIntVar(ref));
  if (!hasDomRefl) {
    GenericSpace *s = extVar2Var(this)->getBoardInternal()->getGenericSpace(true);
    // post the dom reflector propagator
    s->setVarRef(index,ref);
    Gecode::Int::IntVarImp *ivp = reinterpret_cast<Gecode::Int::IntVarImp*>(s->getVar(index));
    GeView<Gecode::Int::IntVarImp> iv(ivp);
    Gecode::Int::IntView *vv = reinterpret_cast<Gecode::Int::IntView*>(&iv);
    new (s) VarInspector<Gecode::Int::IntView, Gecode::Int::PC_INT_DOM>(s,*vv,index);
    s->incForeignProps();
    hasDomRefl = true;
  }
}


#include <iostream>
#include <sstream>
#include <string>

void GeIntVar::printStreamV(ostream &out,int depth) {

  std::stringstream oss;
  oss << getIntVarInfo();
  out << "<GeIntVar " << oss.str().c_str() << ">"; 
}

void module_init_geintvar(void){
  
}

#define STATICALLY_INCLUDED
#include "modGeIntVar-table.cc"
