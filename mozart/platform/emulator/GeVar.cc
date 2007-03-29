#include "GeVar.hh"

using namespace Gecode;

OZ_Return GeVar::unifyV(TaggedRef* lPtr, TaggedRef* rPtr)
{
  printf("unifyV GeVar\n");fflush(stdout);
  
  GeVar* lgevar = this;
  GenericSpace* space = extVar2Var(lgevar)->getBoardInternal()->getGenericSpace();
  
  
  /* the real condition we need to check here is:
     if (oz_isSimpleVar(*rPtr) || oz_isOptVar(*rPtr))
  */
  if (!oz_isGeVar(*rPtr)) {
      /* This is the case when right var is either a simple var or an
	 optimized var and this represents a global var.
      */
    oz_bindGlobalVar2(tagged2Var(*rPtr),rPtr,makeTaggedRef(lPtr));
    goto PROP;
  }
  
  
  { 
    GeVar* rgevar = get_GeVar(*rPtr);
    if(am.inEqEq()) {
      if( IsEmptyInter(lPtr, rPtr) ) return FAILED;
      else {
	trail.pushBind(lPtr);
	trail.pushBind(rPtr);
	return PROCEED;
      }
    }
    
    Assert(space);
    if(oz_isLocalVar(extVar2Var(lgevar))){
      if(!oz_isLocalVar(extVar2Var(rgevar))){
	if(!(rgevar->intersect(makeTaggedRef(lPtr))))
	  return FAILED;
	////	printf("local, no local \n");fflush(stdout);
	oz_bindGlobalVar2(extVar2Var(rgevar), rPtr, makeTaggedRef(lPtr));	
      }else{
	////	printf("unifyV GeIntVar this is local\n");fflush(stdout);
	  // "this" is local.  The binding cannot go upwards, so...    
	  //Assert(oz_isLocalVar(extVar2Var(rgeintvar)));
	  //I don't think this assert is correct,  other can be a global var
	  //printf("local, local \n");fflush(stdout);
	if(!(rgevar->intersect(makeTaggedRef(lPtr))))
	  return FAILED;
	oz_bindLocalVar(extVar2Var(this), lPtr, makeTaggedRef(rPtr));
	propagator(space,lgevar,rgevar);
	lgevar->incUnifyC();
	rgevar->incUnifyC();
	space->incUnifyProps();
      }
    }else{
      // "this" is global.
      if (oz_isLocalVar(extVar2Var(rgevar))) {
	  if(!(lgevar->intersect(makeTaggedRef(rPtr))))
	    return FAILED;
	  //printf("global, local \n");fflush(stdout);
	  oz_bindGlobalVar2(extVar2Var(lgevar), lPtr, makeTaggedRef(rPtr));	
      } else {
	//printf("global, global \n");fflush(stdout);
	OZ_Term lv = newVar();
	
	//TaggedRef glv_tmp = oz_deref(makeTaggedVar(extVar2Var(get_GeIntVar(lv))));
	if(!(lgevar->intersect(lv)))
	  return FAILED;
	if(!(rgevar->intersect(lv)))
	  return FAILED;	
	Assert(oz_isRef(lv));
	oz_bindGlobalVar2(extVar2Var(lgevar), lPtr, lv);	
	oz_bindGlobalVar2(extVar2Var(rgevar), rPtr, lv);	
	}
    }
    
  }
  /* Unification is entailed by means of an eq propagator. After post this
     propagator the generic space must become unstable. The unstability is
     a result of posting the propagator */
  
 PROP:
  //wakeup space propagators to inmediatly update all related variables
  if(space->status() == SS_FAILED) {
    extVar2Var(this)->getBoardInternal()->setFailed();
    return FAILED;
  }
  else
    return PROCEED;
}


OZ_Return GeVar::bindV(TaggedRef* vPtr, TaggedRef val) {
  if (validV(val)) {
    if(am.inEqEq()) {
      if( !In(val) ) return FAILED;
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
      ModEvent me = bind(s,this,val);
      Assert(!me_failed(me));
      
      //unsigned long alt = 0; //useless variable
      
      if (s->status() == SS_FAILED) {
	extVar2Var(this)->getBoardInternal()->setFailed();
	return FAILED;
      }
      return PROCEED;
    } else {
      // global binding...
      oz_bindGlobalVar(extVar2Var(this), vPtr, val);
    }
    return PROCEED;
  }
  return OZ_FAILED;
}
