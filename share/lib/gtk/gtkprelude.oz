%!/usr/bin/perl
%
% Authors:
%   Andreas Simon (2000)
%
% Copyright:
%   Andreas Simon (2000)
%
% Last change:
%   $Date$
%   $Revision$
%
% This file is part of Mozart, an implementation
% of Oz 3:
%   http://www.mozart-oz.org
%
% See the file "LICENSE" or
%   http://www.mozart-oz.org/LICENSE.html
% for information on usage and redistribution
% of this file, and for a DISCLAIMER OF ALL
% WARRANTIES.
%

functor

import
   GtkNative at 'GTK.so{native}'
   System

export
   Dispatcher
   GetObject

   \insert 'gtkimportlist.oz'

define

% -----------------------------------------------------------------------------
% Object Registry
% -----------------------------------------------------------------------------

   % stores GTK object --> OZ object corrospondence
   ObjectRegistry = {Dictionary.new $}

   % Get the corrosponding Oz object from a GTK object
   proc {GetObject MyForeignPointer ?MyObject}
      if MyForeignPointer == 0 then
	 MyObject = nil
      else
	 {Dictionary.get
	  ObjectRegistry
	  {ForeignPointer.toInt MyForeignPointer}
	  MyObject}
      end
   end

% -----------------------------------------------------------------------------
% Dispatcher 
% -----------------------------------------------------------------------------

   local
      PollingIntervall = 50
   in
      class DispatcherClass
	 attr
	    id_counter : 0
	    registry % A dictionary with id <--> handler corrospondences
	    port
	    stream
	    fillerThread
	 meth init
	    proc {FillStream}
	       {GtkNative.handlePendingEvents} % Send all pending GTK events to the Oz port
	       {Time.delay PollingIntervall}
	       {FillStream}
	    end
	 in
	    registry <- {Dictionary.new}
	    port     <- {Port.new @stream}
	    {GtkNative.initializeSignalPort @port} % Tell the 'C side' about the signal port
	    thread
	       fillerThread <- {Thread.this $}
	       {FillStream}
	    end
	    {System.show 'leaving SignalDispatcher init'}
	 end
	 meth GetUniqueID($)
	    id_counter <- @id_counter + 1
	    @id_counter
	 end
	 meth registerHandler(Handler ?Id)
	 {self GetUniqueID(Id)}
	    {Dictionary.put @registry Id Handler}
	 end
	 meth unregisterHandler(Id)
	    {Dictionary.remove @registry Id}
	 end
	 meth dispatch
	    Handler
	    Event
	    Tail
	 in
	    @stream = Event|Tail
	    {Dictionary.get @registry Event Handler}
            % TODO: suspend marshaller with sending a new variable to a second port
	    {Handler}
	    % TODO: terminate marshaller with bounding this variable
	    stream <- Tail
	    DispatcherClass,dispatch
	 end
	 meth exit
	    {Thread.terminate {Thread.this $}}
	 end
      end
   end


   % Start the dispatcher
   Dispatcher = {New DispatcherClass init}
   {System.show 'going to start dispatcher'}
   thread {Dispatcher dispatch} end
   {System.show 'dispatcher is running ...'}

end