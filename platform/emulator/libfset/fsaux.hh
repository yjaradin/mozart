/*
  Hydra Project, DFKI Saarbruecken,
  Stuhlsatzenhausweg 3, D-66123 Saarbruecken, Phone (+49) 681 302-5312
  Author: tmueller
  Last modified: $Date$ from $Author$
  Version: $Revision$
  State: $State$

  ------------------------------------------------------------------------
*/

#include "../oz_cpi.hh" // TMUELLER

#ifndef __FSAUX_HH__
#define __FSAUX_HH__

//#define OZ_DEBUG
//#define DEBUG_INDICES

//-----------------------------------------------------------------------------
// debug macros

#ifdef DEBUG_FSET
#define OZ_DEBUG
#endif

#ifdef OZ_DEBUG
#define OZ_DEBUGCODE(C) C
#define _OZ_DEBUGPRINT(C) (*cpi_cout) << C << endl << flush 
#define OZ_DEBUGPRINT(C) /*_OZ_DEBUGPRINT(C) */
#define OZ_ASSERT(C)						\
  if (! (C)) {							\
    cerr << "OZ_ASSERT " << #C << " failed (" __FILE__ << ':'   \
	 << __LINE__ << ")." << endl << flush;	                \
  }
#define _OZ_DEBUGRETURNPRINT(X) __debugReturnPrint(X)
#define OZ_DEBUGRETURNPRINT(X) X /* _OZ_DEBUGRETURNPRINT(X) */
#else
#define OZ_DEBUGCODE(C)
#define _OZ_DEBUGPRINT(C)
#define OZ_DEBUGPRINT(C)
#define OZ_ASSERT(C)
#define OZ_DEBUGRETURNPRINT(X) X
#define _OZ_DEBUGRETURNPRINT(X) X
#endif

inline
OZ_Return __debugReturnPrint(OZ_Return r) 
{
  *cpi_cout << "returning: ";
  switch (r) {
  case OZ_FAILED: 
    *cpi_cout << "FAILED";
    break;
  case OZ_ENTAILED:
    *cpi_cout << "ENTAILED";
    break;
  case OZ_SLEEP:
    *cpi_cout <<"SLEEP";
    break;
  default:
    *cpi_cout << "??? (" << r << ")";
    break;
  }
  *cpi_cout << endl << flush;
  return r;
}
//-----------------------------------------------------------------------------

#define FailOnEmpty(X) if((X) == 0) goto failure;
#define FailOnInvalid(X) if(!(X)) goto failure;

class PropagatorExpect;

typedef OZ_expect_t (PropagatorExpect::*PropagatorExpectMeth) (OZ_Term);

class PropagatorExpect : public OZ_Expect {
public:
  OZ_expect_t expectIntVarAny(OZ_Term t) { 
    return expectIntVar(t); 
  }
  OZ_expect_t expectIntVarMinMax(OZ_Term t) {
    return expectIntVar(t, fd_prop_bounds); 
  }
  OZ_expect_t expectFSetVarAny(OZ_Term t) { 
    return expectFSetVar(t, fs_prop_any); 
  }
  OZ_expect_t expectFSetVarGlb(OZ_Term t) { 
    return expectFSetVar(t, fs_prop_glb); 
  }
  OZ_expect_t expectFSetVarLub(OZ_Term t) { 
    return expectFSetVar(t, fs_prop_lub); 
  }
  OZ_expect_t expectFSetVarBounds(OZ_Term t) { 
    return expectFSetVar(t, fs_prop_bounds); 
  }
  OZ_expect_t expectVector(OZ_Term t, PropagatorExpectMeth expectf) {
    return OZ_Expect::expectVector(t, (OZ_ExpectMeth) expectf);
  }
  OZ_expect_t expectVectorIntVarMinMax(OZ_Term t) {
    return expectVector(t, &PropagatorExpect::expectIntVarMinMax);
  }
};

//-----------------------------------------------------------------------------

class PropagatorController_S_S {
protected:
  OZ_FSetVar &v1, &v2;
public:
  PropagatorController_S_S(OZ_FSetVar &i1, OZ_FSetVar &i2) 
    : v1(i1), v2(i2) {}

  OZ_Return leave(void) {
    return (v1.leave() | v2.leave()) ? OZ_SLEEP : OZ_ENTAILED;
  }
  OZ_Return leave1(void) {
    int r1 = v1.leave() ? 1 : 0;
    int r2 = v2.leave() ? 1 : 0;
    return (r1 + r2 <= 0) ? OZ_ENTAILED : OZ_SLEEP; // TMUELLER
  }
  OZ_Return vanish(void) {
    v1.leave();
    v2.leave();
    return OZ_ENTAILED;
  }
  OZ_Return fail(void) {
    v1.fail();
    v2.fail();
    return OZ_FAILED;
  }
};

class PropagatorController_S_S_S {
protected:
  OZ_FSetVar &v1, &v2, &v3;
public:
  PropagatorController_S_S_S(OZ_FSetVar &i1, OZ_FSetVar &i2, OZ_FSetVar &i3)
    : v1(i1), v2(i2), v3(i3) {}

  OZ_Return leave(void) {
    return (v1.leave()|v2.leave()|v3.leave())  ? OZ_SLEEP : OZ_ENTAILED;
  }
  OZ_Return leave1(void) {
    int r1 = v1.leave() ? 1 : 0;
    int r2 = v2.leave() ? 1 : 0;
    int r3 = v3.leave() ? 1 : 0;
    return (r1 + r2 + r3 <= 0) ? OZ_ENTAILED : OZ_SLEEP; // TMUELLER
  }
  OZ_Return vanish(void) {
    v1.leave();
    v2.leave();
    v3.leave();
    return OZ_ENTAILED;
  }
  OZ_Return fail(void) {
    v1.fail();
    v2.fail();
    v3.fail();
    return OZ_FAILED;
  }
};

class PropagatorController_S_D {
protected:
  OZ_FSetVar  &v1;
  OZ_FDIntVar &v2;
public:
  PropagatorController_S_D(OZ_FSetVar &i1, OZ_FDIntVar &i2) 
    : v1(i1), v2(i2) {}

  OZ_Return leave(void) {
    return (v1.leave() | v2.leave()) ? OZ_SLEEP : OZ_ENTAILED;
  }
  OZ_Return leave1(void) {
    int r1 = v1.leave() ? 1 : 0;
    int r2 = v2.leave() ? 1 : 0;
    return (r1 + r2 <= 1) ? OZ_ENTAILED : OZ_SLEEP;
  }
  OZ_Return vanish(void) {
    v1.leave();
    v2.leave();
    return OZ_ENTAILED;
  }
  OZ_Return fail(void) {
    v1.fail();
    v2.fail();
    return OZ_FAILED;
  }
};

class PropagatorController_S_VD {
protected:
  OZ_FSetVar  & _s;
  OZ_FDIntVar * _vd;
  int _vd_size;
public:
  PropagatorController_S_VD(OZ_FSetVar &s, int vd_size, OZ_FDIntVar vd[]) 
    : _s(s), _vd_size(vd_size), _vd(vd){}

  OZ_Return leave(void) {
    OZ_Boolean vars_left = _s.leave();
    for (int i = _vd_size; i--; vars_left |= _vd[i].leave());
    return vars_left ? OZ_SLEEP : OZ_ENTAILED;
  }
  OZ_Return vanish(void) {
    _s.leave();
    for (int i = _vd_size; i--; _vd[i].leave());
    return OZ_ENTAILED;
  }
   OZ_Return fail(void) {
    _s.fail();
    for (int i = _vd_size; i--; _vd[i].fail());
    return OZ_FAILED;
  }
};

//-----------------------------------------------------------------------------
// Dynamically sized arrays for compilers which do not provide alloca


#ifdef __GNUC__
#define USE_GCCALLOCA
#else
#ifdef __XXWATCOMC__
#define USE_INTVAR_NEW
#else
#ifdef _MSC_VER
#define USE_TEMPLATE_ARRAY
#else
#define USE_TEMPLATE_ARRAY
#endif
#endif
#endif


#ifdef DEBUG_INDICES

#include <stdlib.h>

template <class T>
class IndexCheckArray {
private:
  int _size;
  T * _array;
public:
  IndexCheckArray(int s)  { 
    _size = s; 
    _array = (T *) malloc(s * sizeof(T));
  }
  
  inline
  T &operator [](int i) { 
    OZ_ASSERT(0 <= i && i < _size);
    return _array[i]; 
  }
  
  inline
  operator T*() { return _array; } // conversion operator
};

#define _DECL_DYN_ARRAY(Type,Var,Size) IndexCheckArray<Type> Var(Size)
#else

/* gcc supports dynamic sized arrays */
#ifdef USE_GCCALLOCA
#define _DECL_DYN_ARRAY(Type,Var,Size) Type Var[Size]
#endif


#ifdef USE_INTVAR_NEW
#define _DECL_DYN_ARRAY(Type,Var,Size) \
     Type *Var = (Type *) OZ_FDIntVar::operator new(sizeof(Type) * Size)
#endif

#ifdef USE_ALLOCA
#define _DECL_DYN_ARRAY(Type,Var,Size) \
     Type *Var = (Type *) alloca(sizeof(Type) * Size)
#endif


/* this one is really slooow */
#ifdef USE_TEMPLATE_ARRAY

#define _DECL_DYN_ARRAY(Type,Var,Size) _DynArray<Type> Var(Size)

void * freeListMalloc(size_t chunk_size);
void freeListDispose(void *addr, size_t chunk_size);

template <class T>
class _DynArray {
private:
  T * array;
  int size;
public:
  _DynArray(int sz) { size = sz; array = (T*) freeListMalloc(sz*sizeof(T)); };
  ~_DynArray() { freeListDispose(array,size*sizeof(T)); }
  T &operator[] (int i) { return array[i]; }
  operator T * () { return array; }
};

#endif
#endif /* DEBUG_INDICES */

/* cannot handle sometimes arrays of size 0 correctly */
#define DECL_DYN_ARRAY(Type,Var,Size) \
_DECL_DYN_ARRAY(Type,Var,Size==0?1:Size) 

//*****************************************************************************

class FSetTouched {
private:
  int _known_in, _known_not_in, _card_size;

public:
  FSetTouched(void) {};

  void operator = (OZ_FSetVar &sv) 
  {
    _known_in = sv->getKnownIn();
    _known_not_in = sv->getKnownNotIn();
    _card_size = sv->getCardSize();
  }

  OZ_Boolean operator <= (OZ_FSetVar &sv) 
  {
    if ((_known_in < sv->getKnownIn()) ||
	(_known_not_in < sv->getKnownNotIn()) ||
	(_card_size > sv->getCardSize()))
      return OZ_TRUE;

    return OZ_FALSE;
  }
};

#define RETURN_LIST1(X) \
return OZ_cons(X, OZ_nil())

#define RETURN_LIST2(X, Y) \
return OZ_cons(X, OZ_cons(Y, OZ_nil()))

#define RETURN_LIST3(X, Y, Z) \
return OZ_cons(X, OZ_cons(Y, OZ_cons(Z, OZ_nil())))

#define RETURN_LIST4(U, V, W, X) \
return OZ_cons(U, OZ_cons(V, OZ_cons(W, OZ_cons(X, OZ_nil()))))

#define RETURN_LIST5(U, V, W, X, Y) \
return OZ_cons(U, OZ_cons(V, OZ_cons(W, OZ_cons(X, OZ_cons(Y, OZ_nil())))))

#define _TERMVECTOR2LIST(V, S, L, C)		\
OZ_Term L = OZ_nil();				\
{for (int i = S; i--; )				\
  L = OZ_cons(C(V[i]), L); }

     
#define TERMVECTOR2LIST(V, S, L)  _TERMVECTOR2LIST(V, S, L, )
#define INTVECTOR2LIST(V, S, L)  _TERMVECTOR2LIST(V, S, L, OZ_int)


class FSetIterator {
private:
  OZ_FSetValue * _fset;
  int _elem;

public:
  FSetIterator(OZ_FSetValue * fs) : _fset(fs) {}
  FSetIterator(OZ_FSetValue * fs, int elem) : _elem(elem), _fset(fs) {}

  void init(int elem) { _elem = elem; }

  int resetToMin(void) {
    return _elem = _fset->getMinElem();
  }
  int resetToMax(void) {
    return _elem = _fset->getMaxElem();
  }

  int getNextLarger(void) {
    return _elem = _fset->getNextLargerElem(_elem);
  }
  int getNextSmaller(void) {
    return _elem = _fset->getNextSmallerElem(_elem);
  }
};

//-----------------------------------------------------------------------------

inline 
int min(int a, int b) { return a < b ? a : b; }

inline 
int max(int a, int b) { return a > b ? a : b; }

#endif /* __FSAUX_HH__ */

//-----------------------------------------------------------------------------
// eof
