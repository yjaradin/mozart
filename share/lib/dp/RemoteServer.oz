%%%
%%% Authors:
%%%   Christian Schulte (schulte@dfki.de)
%%%
%%% Copyright:
%%%   Christian Schulte, 1998
%%%
%%% Last change:
%%%   $Date$ by $Author$
%%%   $Revision$
%%%
%%% This file is part of Mozart, an implementation
%%% of Oz 3
%%%    $MOZARTURL$
%%%
%%% See the file "LICENSE" or
%%%    $LICENSEURL$
%%% for information on usage and redistribution
%%% of this file, and for a DISCLAIMER OF ALL
%%% WARRANTIES.
%%%

proc {RemoteServer RunRet CtrlRet Close}
   RunStr CtrlStr
in
   {Port.send RunRet  {Port.new RunStr}}
   {Port.send CtrlRet {Port.new CtrlStr}}
   
   %% The server for running procedures
   thread
      {ForAll RunStr
       proc {$ P}
	  {Port.send RunRet
	   try
	      X = case {Procedure.is P} then {P}
		  else raise gaga end
		  end
	   in
	      okay(X)
	   catch E then
	      exception(E)
	   end}
       end}
   end
   
   %% The server for control messages
   thread
      {ForAll CtrlStr
       proc {$ C}
	  {Port.send CtrlRet
	   okay(case C
		of ping  then unit
		[] close then {Close} unit
		end)}
       end}
   end
   
end
