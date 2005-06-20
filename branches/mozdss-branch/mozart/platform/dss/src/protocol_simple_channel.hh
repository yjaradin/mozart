/*
 *  Authors:
 *    Erik Klintskog (erik@sics.se)
 * 
 *  Contributors:
 *    optional, Contributor's name (Contributor's email address)
 * 
 *  Copyright:
 *    Erik Klintskog, 2002
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
#ifndef __PROTOCOL_SIMPLE_CHANNEL_HH
#define __PROTOCOL_SIMPLE_CHANNEL_HH

#ifdef INTERFACE
#pragma interface
#endif

#include "dssBase.hh"
#include "protocols.hh"
namespace _dss_internal{ //Start namespace


  class ProtocolSimpleChannelManager:public ProtocolManager{
    ProtocolSimpleChannelManager(const ProtocolSimpleChannelManager&){};
    ProtocolSimpleChannelManager& operator=(const ProtocolSimpleChannelManager&){ return *this; }
  public:
    ProtocolSimpleChannelManager();
    ProtocolSimpleChannelManager(::MsgContainer *);
    ~ProtocolSimpleChannelManager(){};

    void msgReceived(::MsgContainer*,DSite*);    
    void sendMigrateInfo(::MsgContainer*); 
  };
  
  
  class ProtocolSimpleChannelProxy:public ProtocolProxy{
    friend class _dss_internal::ProtocolSimpleChannelManager;
    bool stateHolder; 
  public: 
    ProtocolSimpleChannelProxy();
    ~ProtocolSimpleChannelProxy(){};
    
    OpRetVal protocol_Synch(GlobalThread* const th_id, ::PstOutContainerInterface**& msg, const AbsOp& aop);
    OpRetVal protocol_Asynch(::PstOutContainerInterface**& msg, const AbsOp& aop);
    
    void remoteInitatedOperationCompleted(DssOperationId*, ::PstOutContainerInterface*); 
    void localInitatedOperationCompleted(); 
    
    
    bool isWeakRoot(){ return stateHolder; }; // The glue should know if a thread is relying on proxy

    void msgReceived(::MsgContainer*,DSite*);
    virtual bool m_initRemoteProt(DssReadBuffer*); 

  };


} //End namespace
#endif
