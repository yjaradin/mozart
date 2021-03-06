/*
 *  Authors:
 *    Zacharias El Banna (zeb@sics.se)
 * 
 *  Contributors:
 *    Raphael Collet (raph@info.ucl.ac.be)
 * 
 *  Copyright:
 *    Zacharias El Banna, 2002
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

#if defined(INTERFACE)
#pragma implementation "protocol_migratory.hh"
#endif

#include "protocol_migratory.hh"
#include "dssBase.hh"

namespace _dss_internal{ //Start namespace

  // Quick description of the protocol.
  //
  // The protocol makes a unique token migrate among proxies, and the
  // state of the abstract entity migrates with the token.  The proxy
  // that has the token can update the entity locally.  The protocol
  // dynamically builds a forwarding chain among the proxies that want
  // to update the abstract entity.
  //
  // Proxy P asks for the token:
  //    P              M                   P'
  //    |---MIGM_GET-->|                   |   P' is the last proxy
  //    |              |--MIGM_FORWARD(P)->|   in the forwarding chain
  //
  // Proxy P forwards the token (after reception):
  //    P                  P'
  //    |----MIGM_TOKEN--->|
  //
  // Proxy P has the token but no successor:
  //    P                     M
  //    |---MIGM_TOKEN_HERE-->|   (not mandatory, optimization only)
  //
  // Proxy P wants to get rid of the token:
  //    P                       M
  //    |---MIGM_NEED_NO_MORE-->|   (P has the token and no successor)
  //
  // The following part extends the protocol above in order to avoid
  // failed proxies, and diagnose the loss of the token.
  //
  // P detects that its successor P' failed:
  //    P                      M
  //    |---MIGM_FAILED_SUCC-->|
  //    |<--MIGM_FORWARD(P")---|   if P" is the successor of P'
  //
  // Manager inquires the predecessor of a failed proxy:
  //    M                     P
  //    |---MIGM_CHECK_SUCC-->|
  //    |<---MIGM_OLD_SUCC----|   if P already forwarded token
  // or:
  //    |<--MIGM_FAILED_SUCC--|   if P has not forwarded yet
  //
  // Manager inquires the successor of a failed proxy (only when the
  // latter has no predecessor):
  //    M                     P
  //    |---MIGM_CHECK_PRED-->|
  //    |<---MIGM_OLD_PRED----|   if P already forwarded token
  // or:
  //    |<--MIGM_TOKEN_HERE---|   if P has the token
  // or:
  //    |<--MIGM_FAILED_PRED--|   if P has not received it yet
  //
  // In the latter reply, we can infer that P will never receive the
  // token.  It is therefore lost, and the manager notifies proxies
  // with PROT_PERMFAIL.

  namespace {
    // messages
    enum Migratory_Message {
      MIGM_GET,          // PM: request access
      MIGM_FORWARD,      // MP: send the successor of a proxy
      MIGM_TOKEN,        // PP: pass to token from proxy to proxy
      MIGM_TOKEN_HERE,   // PM: informs manager
      MIGM_NEED_NO_MORE, // PM: please remove the token from here
      MIGM_CHECK_SUCC,   // MP: manager inquiring to find token
      MIGM_CHECK_PRED,   // MP: manager inquiring to find token
      MIGM_FAILED_SUCC,  // PM: proxy will not forward to its successor
      MIGM_FAILED_PRED,  // PM: proxy will not receive from this successor
      MIGM_OLD_SUCC,     // PM: reply to an old request
      MIGM_OLD_PRED      // PM: reply to an old request
    };
  }



  /******************** ProtocolMigratoryManager ********************/

  ProtocolMigratoryManager::ProtocolMigratoryManager(DSite* mysite) {
    a_chain.append(makePair(mysite, 0));
    a_last = mysite;    // the token is at home
  }


  void
  ProtocolMigratoryManager::sendMigrateInfo(::MsgContainer* msg) {
    ProtocolManager::sendMigrateInfo(msg);
    for (QueuePosition<Pair<DSite*, int> > p(a_chain); p(); p++) {
      msg->pushDSiteVal((*p).first);
      msg->pushIntVal((*p).second);
    }
  }

  ProtocolMigratoryManager::ProtocolMigratoryManager(::MsgContainer* msg) :
    ProtocolManager(msg) {
    a_last = NULL;
    while (!msg->m_isEmpty()) {
      a_last = msg->popDSiteVal();
      a_chain.append(makePair(a_last, msg->popIntVal()));
    }
  }


  void
  ProtocolMigratoryManager::msgReceived(::MsgContainer* msg, DSite* sender) {
    int msgType = msg->popIntVal();
    switch (msgType) {
    case PROT_REGISTER: {
      if (isPermFail()) sendToProxy(sender, PROT_PERMFAIL);
      else registerProxy(sender);
      break;
    }
    case PROT_DEREGISTER: {
      deregisterProxy(sender);
      break;
    }
    case MIGM_GET: {
      if (isPermFail()) sendToProxy(sender, PROT_PERMFAIL);
      else if (sender != a_last) {
	int reqid = msg->popIntVal();
	sendToProxy(a_last, MIGM_FORWARD, sender);
	if (a_chain.front().find(sender)) {
	  // remove everything up to the sender
	  while (a_chain.pop().first != sender) ;
	}
	a_chain.append(makePair(sender, reqid));
	a_last = sender;
      }
      break;
    }
    case MIGM_TOKEN_HERE: {
      if (!isPermFail() && a_chain.front().find(sender)) {
	QueuePosition<Pair<DSite*,int> > p(a_chain);
	// remove all elements before sender
	while ((*p).first != sender) p.pop();
      }
      break;
    }
    case MIGM_NEED_NO_MORE:{
      if (sender == a_last) {
	// The sender has no successor; force the home proxy to
	// request the state.
	ProtocolProxy* pp = a_coordinator->m_getProxy()->m_getProtocol();
	static_cast<ProtocolMigratoryProxy*>(pp)->requestToken();
      }
      break;
    }
    case MIGM_FAILED_SUCC: {
      // a proxy notifies that its successor has failed
      if (!isPermFail()) {
	QueuePosition<Pair<DSite*,int> > p(a_chain);
	p.find(sender); p++;     // p is on the sender's successor
	Assert(p());
	p.remove();              // remove the successor
	if (p()) { // forward to the new successor
	  sendToProxy(sender, MIGM_FORWARD, (*p).first);
	} else { // we removed the last proxy in a_chain
	  a_chain.check();
	  a_last = sender;
	}
      }
      break;
    }
    case PROT_PERMFAIL:
    case MIGM_FAILED_PRED: {
      // The sender tells that it will not receive the token; it is
      // therefore lost (we know the predecessor has had it).
      if (!isPermFail()) lostToken();
      break;
    }
    case MIGM_OLD_SUCC:
    case MIGM_OLD_PRED: {
      // The sender informs the manager that it no longer has the
      // state (for the given request).
      if (!isPermFail()) {
	int reqid = msg->popIntVal();
	QueuePosition<Pair<DSite*,int> > p(a_chain);
	if (p.find(sender) && (*p).second == reqid) {
	  // drop all elements up to sender
	  while (a_chain.pop().first != sender) ;
	  
	  if (msgType == MIGM_OLD_SUCC) {
	    // see whether a successor has had the token
	    inquire(a_chain.front().element().first);
	  }
	}
      }
      break;
    }
    default:
      Assert(0);
    }
  }


  // inquiry protocol around a failed proxy
  void ProtocolMigratoryManager::inquire(DSite* s) {
    QueuePosition<Pair<DSite*,int> > cur(a_chain);
    bool found = false;
    QueuePosition<Pair<DSite*,int> > other;
    
    // first try to find the closest non-failed predecessor
    while ((*cur).first != s) {
      if ((*cur).first->m_getFaultState() <= FS_TEMP)
	found = true, other = cur;
      cur++;
    }
    if (found) { // we have a predecessor, inquire it
      sendToProxy((*other).first, MIGM_CHECK_SUCC, (*other).second);
      return;
    }
    
    // try to find the closest non-failed successor, then
    cur++;
    while (cur()) {
      if ((*cur).first->m_getFaultState() <= FS_TEMP)
	found = true, other = cur;
      cur++;
    }
    if (found) { // we have a successor, inquire it
      sendToProxy((*other).first, MIGM_CHECK_PRED, (*other).second);
      return;
    }
    
    // all proxies in the chain failed; we've lost the state
    while (!a_chain.isEmpty()) a_chain.pop();
    lostToken();
  }

  // notify proxies that the token has been lost
  void ProtocolMigratoryManager::lostToken() {
    a_last = NULL;
    // notify all proxies left in a_chain
    while (!a_chain.isEmpty()) {
      DSite* s = a_chain.pop().first;
      sendToProxy(s, PROT_PERMFAIL);
      deregisterProxy(s);     // don't notify it twice...
    }
    // notify all registered proxies
    makePermFail();
  }


  // check for failed proxies
  void ProtocolMigratoryManager::m_siteStateChange(DSite* s,
						   const FaultState& state) {
    ProtocolManager::m_siteStateChange(s, state);
    if (!isPermFail() && state == FS_GLOBAL_PERM && a_chain.front().find(s))
      inquire(s);
  }



  /******************** ProtocolMigratoryProxy ********************/

  ProtocolMigratoryProxy::ProtocolMigratoryProxy() :
    ProtocolProxy(PN_MIGRATORY_STATE), a_successor(NULL), a_request(0)
  { setStatus(MIGT_HERE); }

  ProtocolMigratoryProxy::~ProtocolMigratoryProxy() {
    if (!a_proxy->m_isHomeProxy()) protocol_Deregister();
  }
  
  bool 
  ProtocolMigratoryProxy::m_initRemoteProt(DssReadBuffer*) {
    setStatus(MIGT_EMPTY);
    return false;
  }

  void
  ProtocolMigratoryProxy::makeGCpreps() {
    ProtocolProxy::makeGCpreps();
    if (a_successor) a_successor->m_makeGCpreps();
  }


  void
  ProtocolMigratoryProxy::requestToken(){
    Assert(getStatus() == MIGT_EMPTY);
    dssLog(DLL_BEHAVIOR,"MigratoryProxy::requestToken");
    sendToManager(MIGM_GET, a_request);
    setStatus(MIGT_REQUESTED);
  }

  void
  ProtocolMigratoryProxy::forwardToken(){
    Assert(getStatus() == MIGT_HERE && a_successor);
    dssLog(DLL_BEHAVIOR,"MigratoryProxy::forwardToken : To:%s",
	   a_successor->m_stringrep());
    sendToProxy(a_successor, MIGM_TOKEN, a_proxy->deinstallEntityState());
    setStatus(MIGT_EMPTY);
    a_successor = NULL;
    a_request++;
  }

  void
  ProtocolMigratoryProxy::resumeOperations(){
    Assert(!a_susps.isEmpty() || a_proxy->m_isHomeProxy());
    // wake up all suspended threads
    while (!a_susps.isEmpty())
      a_susps.pop()->resumeDoLocal();
    // forward the token
    if (a_successor)
      forwardToken();
    else
      sendToManager(MIGM_TOKEN_HERE);
  }

  void
  ProtocolMigratoryProxy::lostToken() {
    makePermFail();
    a_successor = NULL;
  }


  OpRetVal
  ProtocolMigratoryProxy::protocol_Access(GlobalThread* const id,
					  ::PstOutContainerInterface**& out){
    dssLog(DLL_BEHAVIOR,"MigratoryProxy::Access operation (%d)",getStatus());
    if (isPermFail()) return DSS_RAISE;
    out = NULL; // Never transmit
    switch(getStatus()){
    case MIGT_HERE:
      return DSS_PROCEED;
    case MIGT_EMPTY:
      requestToken();
      // fall through
    case MIGT_REQUESTED:
      a_susps.append(id);
      return DSS_SUSPEND;
    default:
      Assert(0);
      return DSS_INTERNAL_ERROR_SEVERE; // Whatever....
    }
  }

  OpRetVal
  ProtocolMigratoryProxy::operationRead(GlobalThread* thr,
					PstOutContainerInterface**& out) {
    return protocol_Access(thr, out);
  }

  OpRetVal
  ProtocolMigratoryProxy::operationWrite(GlobalThread* thr,
					 PstOutContainerInterface**& out) {
    return protocol_Access(thr, out);
  }

  
  void
  ProtocolMigratoryProxy::msgReceived(::MsgContainer* msg, DSite* sender){
    if (isPermFail()) return;
    int msgType = msg->popIntVal();
    switch (msgType) {
    case MIGM_FORWARD:{
      Assert(a_successor == NULL);
      a_successor = msg->popDSiteVal();
      if (getStatus() == MIGT_HERE) forwardToken();
      break;
    }
    case MIGM_TOKEN:{
      setStatus(MIGT_HERE);
      ::PstInContainerInterface* builder = gf_popPstIn(msg);
      a_proxy->installEntityState(builder);
      resumeOperations();
      break;
    }
    case MIGM_CHECK_SUCC: {
      int reqid = msg->popIntVal();
      if (a_request != reqid) { // this is an old request
	sendToManager(MIGM_OLD_SUCC, reqid);
      } else if (a_successor) { // ask for a new successor, then!
	a_successor = NULL;
	sendToManager(MIGM_FAILED_SUCC);
      }
      break;
    }
    case MIGM_CHECK_PRED: {
      int reqid = msg->popIntVal();
      if (a_request != reqid) { // this is an old request
	sendToManager(MIGM_OLD_PRED, reqid);
      } else if (getStatus() == MIGT_HERE) { // the token is here
	sendToManager(MIGM_TOKEN_HERE);
      } else { // we will never receive the token
	sendToManager(MIGM_FAILED_PRED);
	lostToken();     // early failure (beware!)
      }
      break;
    }
    case PROT_PERMFAIL: {
      lostToken();
      break;
    }
    default:
      Assert(0);
    }
  }

  bool
  ProtocolMigratoryProxy::clearWeakRoot(){
    // Must check isWeakRoot() again since we don't know if something
    // happened since last check
    if (isWeakRoot()) {
      if (getStatus() == MIGT_HERE) {
	if (a_successor) forwardToken();
	else sendToManager(MIGM_NEED_NO_MORE);
      }
    }
    return isWeakRoot();
  }

  // interpret a site failure
  FaultState
  ProtocolMigratoryProxy::siteStateChanged(DSite* s, const FaultState& state) {
    if (!isPermFail()) {
      if (a_proxy->m_getCoordinatorSite() == s) {
	switch (state) {
	case FS_OK:          return FS_STATE_OK;
	case FS_TEMP:        return FS_STATE_TEMP;
	case FS_LOCAL_PERM:  makePermFail(state); return FS_STATE_LOCAL_PERM;
	case FS_GLOBAL_PERM: lostToken(); return FS_STATE_GLOBAL_PERM;
	default:
	  dssError("Unknown DSite state %d for %s",state,s->m_stringrep());
	}
      }
      if (a_successor == s && state == FS_GLOBAL_PERM) {
	a_successor = NULL;
	sendToManager(MIGM_FAILED_SUCC);
      }
    }
    return 0;
  }

} //End namespace
