/*
 *  Authors:
 *    Denys Duchier (duchier@ps.uni-sb.de)
 *    Christian Schulte (schulte@dfki.de)
 * 
 *  Copyright:
 *    Christian Schulte, 1998
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


#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#include <string.h>

#include "runtime.hh"
#include "builtins.hh"
#include "os.hh"

#ifdef MODULES_LINK_STATIC
#define DYNAMIC_MODULE(m) m
#else
#define DYNAMIC_MODULE(m) 0
#endif



TaggedRef builtinRecord;




#ifdef MODULES_LINK_STATIC
// Declarations for all modules than can be loaded dynamically

#include "modWif.dcl"
#include "modFDP.dcl"
#include "modSchedule.dcl"
#include "modParser.dcl"
#include "modFSP.dcl"

static OZ_C_proc_interface mod_int_Wif[] = {
#include "modWif.tbl"
 {0,0,0,0}
};

static OZ_C_proc_interface mod_int_FDP[] = {
#include "modFDP.tbl"
 {0,0,0,0}
};

static OZ_C_proc_interface mod_int_Schedule[] = {
#include "modSchedule.tbl"
 {0,0,0,0}
};

static OZ_C_proc_interface mod_int_FSP[] = {
#include "modFSP.tbl"
 {0,0,0,0}
};

static OZ_C_proc_interface mod_int_Parser[] = {
#include "modParser.tbl"
 {0,0,0,0}
};

#include "modCompilerSupport.dcl"
static OZ_C_proc_interface mod_int_CompilerSupport[] = {
#include "modCompilerSupport.tbl"
 {0,0,0,0}
};

#include "modBrowser.dcl"
static OZ_C_proc_interface mod_int_Browser[] = {
#include "modBrowser.tbl"
 {0,0,0,0}
};

#include "modDebug.dcl"
static OZ_C_proc_interface mod_int_Debug[] = {
#include "modDebug.tbl"
 {0,0,0,0}
};

#endif


struct ModuleEntry {
  const char *          name;
  OZ_C_proc_interface * interface;
};


#include "modProperty.dcl"
#include "modOS.dcl"
#include "modPickle.dcl"
#include "modURL.dcl"
#include "modPID.dcl"
#include "modFDB.dcl"
#include "modFSB.dcl"
#include "modSystem.dcl"
#include "modCTB.dcl"
#include "modFinalize.dcl"
#include "modProfile.dcl"
#include "modForeign.dcl"
#include "modFault.dcl"
#include "modDistribution.dcl"

static OZ_C_proc_interface mod_int_Property[] = {
#include "modProperty.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_OS[] = {
#include "modOS.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_Pickle[] = {
#include "modPickle.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_URL[] = {
#include "modURL.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_PID[] = {
#include "modPID.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_FDB[] = {
#include "modFDB.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_FSB[] = {
#include "modFSB.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_System[] = {
#include "modSystem.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_CTB[] = {
#include "modCTB.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_Finalize[] = {
#include "modFinalize.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_Profile[] = {
#include "modProfile.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_Foreign[] = {
#include "modForeign.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_Fault[] = {
#include "modFault.tbl"
 {0,0,0,0}
};
static OZ_C_proc_interface mod_int_Distribution[] = {
#include "modDistribution.tbl"
 {0,0,0,0}
};

#ifdef MISC_BUILTINS
#include "modMisc.dcl"
static OZ_C_proc_interface mod_int_Misc[] = {
#include "modMisc.tbl"
 {0,0,0,0}
};
#endif

#ifdef VIRTUALSITES
#include "modVirtualSite.dcl"
static OZ_C_proc_interface mod_int_VirtualSite[] = {
#include "modVirtualSite.tbl"
 {0,0,0,0}
};
#endif


static ModuleEntry module_table[] = {
  // Most important stuff
  {"Property", mod_int_Property},
  {"OS",       mod_int_OS},
  {"URL",      mod_int_URL},
  {"Pickle",   mod_int_Pickle},
  {"System",   mod_int_System},

  {"FDB",      mod_int_FDB},
  {"FSB",      mod_int_FSB},

  {"FSP",              DYNAMIC_MODULE(mod_int_FSP) },
  {"FDP",              DYNAMIC_MODULE(mod_int_FDP) },
  {"CompilerSupport",  DYNAMIC_MODULE(mod_int_CompilerSupport)},
  {"Parser",           DYNAMIC_MODULE(mod_int_Parser) },

  {"Finalize", mod_int_Finalize},
  {"Profile",  mod_int_Profile},

  {"Foreign",      mod_int_Foreign},
  {"Fault",        mod_int_Fault},
  {"Distribution", mod_int_Distribution},

  {"CTB",      mod_int_CTB},
  {"PID",      mod_int_PID},

  {"Browser",          DYNAMIC_MODULE(mod_int_Browser)},
  {"Wif",              DYNAMIC_MODULE(mod_int_Wif) },
  {"Schedule",         DYNAMIC_MODULE(mod_int_Schedule) },
  {"Debug",            DYNAMIC_MODULE(mod_int_Debug)},

#ifdef MISC_BUILTINS
  {"Misc",         mod_int_Misc},
#endif

#ifdef VIRTUALSITES
  {"VirtualSite",  mod_int_VirtualSite},
#endif

  {0, 0},
};


static TaggedRef ozInterfaceToRecord(OZ_C_proc_interface * I, 
				     Bool isSited) {
  OZ_Term l = oz_nil();

  Builtin *bi;

  while (I && I->name) {
    bi = new Builtin(I->name,I->inArity,I->outArity,I->func,isSited);
 
    l = oz_cons(oz_pairA(I->name,makeTaggedConst(bi)),l);
    I++;
  }

  return OZ_recordInit(AtomExport,l);
}


OZ_BI_define(BIBootManager, 1, 1) {
  oz_declareVirtualStringIN(0, mod_name);


  // Check for builtins

  if (!strcmp("Builtins", mod_name)) 
    OZ_RETURN(builtinRecord);

  // First: find module entry in table
  
  ModuleEntry * me = module_table;

  while (me && me->name && strcmp(me->name, mod_name)) {
    me++;
  }

  if (!me || !me->name) {
    fprintf(stderr, "Unknown boot module: %s.\n",mod_name);
    osExit(1);
  }
  
  OZ_C_proc_interface * I = 0;
  
  if (me->interface) {
    // Thats easy, is linked statically
    I = me->interface;
  } else {
    // Not there, try to load dynamically!

    TaggedRef hdl;

    int n = strlen(ozconf.emuhome);
    int m = strlen(mod_name);
  
    char * libfile = new char[n + m + 64];
    
    strcpy(libfile,             ozconf.emuhome);
    strcpy(libfile + n,         "/lib");
    strcpy(libfile + n + 4,     mod_name);
    strcpy(libfile + n + m + 4, ".so");
    
    OZ_Return res = osDlopen(libfile,hdl);
    
    if (res!=PROCEED) {
      fprintf(stderr, "Could not open boot library: %s.\n",libfile);
      osExit(1);
    }

    
    void * handle = OZ_getForeignPointer(hdl);

    char * if_name = new char[m + 16];
    
    strcpy(if_name,     "mod_int_");
    strcpy(if_name + 8, mod_name);

    I = (OZ_C_proc_interface *) osDlsym(handle, if_name);

    if (!I) {
      fprintf(stderr, "Interface %s:\n missing in boot library: %s.\n",
	      if_name, libfile);
      osExit(1);
    }

    delete[] libfile;
    delete[] if_name;
    
  }
  
  
  OZ_RETURN(ozInterfaceToRecord(I,OK));

} OZ_BI_end


OZ_BI_define(BIdlLoad,1,1)
{
  oz_declareVirtualStringIN(0,filename);

  TaggedRef hdl;
  OZ_Return res = osDlopen(filename,hdl);
  if (res!=PROCEED) return res;
  void* handle = OZ_getForeignPointer(hdl);
  OZ_C_proc_interface * I;
  I = (OZ_C_proc_interface *) osDlsym(handle,"oz_interface");
  if (I==0)
    return oz_raise(E_ERROR,AtomForeign, "cannotFindInterface", 1,
		    OZ_in(0));

  OZ_RETURN(oz_pair2(hdl,ozInterfaceToRecord(I,OK)));
} OZ_BI_end

extern void BIinitPerdio();

OZ_C_proc_proto(BIcontrolVarHandler);
OZ_C_proc_proto(BIatRedo);
OZ_C_proc_proto(BIfail);
OZ_C_proc_proto(BIurl_load);
OZ_C_proc_proto(BIload);
OZ_C_proc_proto(BIprobe);
OZ_C_proc_proto(BIstartTmp);
OZ_C_proc_proto(BIportWait);


#include "modBuiltins.dcl"

static OZ_C_proc_interface mod_int_Builtins[] = {
#include "modBuiltins.tbl"
 {0,0,0,0}
};


void initBuiltins() {
  builtinRecord = ozInterfaceToRecord(mod_int_Builtins, NO);

  OZ_protect(&builtinRecord);

  // General stuff
  BI_send         = makeTaggedConst(string2Builtin("Send"));
  BI_exchangeCell = makeTaggedConst(string2Builtin("Exchange"));
  BI_assign       = makeTaggedConst(string2Builtin("<-"));
  BI_lockLock     = makeTaggedConst(string2Builtin("Lock"));
  BI_Delay        = makeTaggedConst(string2Builtin("Delay"));
  BI_Unify        = makeTaggedConst(string2Builtin("="));

  // Exclusively used (not in builtin table)
  BI_controlVarHandler = 
    makeTaggedConst(new Builtin("controlVarHandler", 
				1, 0, BIcontrolVarHandler, OK));


  BIinitPerdio();
  
  
  // Exclusively used (not in builtin table)
  BI_probe     = 
    makeTaggedConst(new Builtin("probe", 
				1, 0, BIprobe, OK));
  BI_startTmp  = 
    makeTaggedConst(new Builtin("startTmp",
				2, 0, BIstartTmp, OK));
  BI_portWait  = 
    makeTaggedConst(new Builtin("portWait", 
				2, 0, BIportWait, OK));
  BI_atRedo    =
    makeTaggedConst(new Builtin("atRedo", 
				2, 0, BIatRedo, OK));
  BI_fail      =
    makeTaggedConst(new Builtin("fail", 
				0, 0, BIfail, OK));

  // if mapping from cfun to builtin fails
  BI_unknown =
    makeTaggedConst(new Builtin("UNKNOWN", 0, 0, BIfail,     OK));
    
  // to execute boot functor in am.cc
  BI_dot      =
    makeTaggedConst(string2Builtin("."));
  // not in builtin table...
  BI_load     = 
    makeTaggedConst(new Builtin("load",     2, 0, BIload,     OK));
  BI_url_load = 
    makeTaggedConst(new Builtin("URL.load", 1, 1, BIurl_load, OK));
  BI_boot_manager =
    makeTaggedConst(new Builtin("BootManager", 1, 1, BIBootManager, OK));


  bi_raise      = string2Builtin("raise");
  bi_raiseError = string2Builtin("raiseError");
  bi_raiseDebug = string2Builtin("raiseDebug");

  initObjectBuiltins();

}

