%%%
%%% Authors:
%%%   Ralf Scheidhauer <scheidhr@dfki.de>
%%%
%%% Copyright:
%%%   Ralf Scheidhauer, 1998
%%%
%%% Last change:
%%%   $Date$ by $Author$
%%%   $Revision$
%%%
%%% This file is part of Mozart, an implementation of Oz 3:
%%%   http://www.mozart-oz.org
%%%
%%% See the file "LICENSE" or
%%%   http://www.mozart-oz.org/LICENSE.html
%%% for information on usage and redistribution
%%% of this file, and for a DISCLAIMER OF ALL
%%% WARRANTIES.
%%%

functor
import
   Pickle(save load saveCompressed)
   OS(tmpnam unlink)
export
   Return
define
   fun {TrySave Val File}
      try
	 {Pickle.save Val File}
	 nil
      catch error(dp(generic 'save:nogoods'   _ ('Resources'#X)|_) ...) then X
      []    error(dp(generic 'save:resources' _ ('Resources'#X)|_) ...) then X
      end
   end

   Nogoods = 
     [{NewCell 1}
      {NewPort _}
      _
      _::0#1
      {New BaseObject noop}
      proc sited {$} skip end
      class $ prop sited end
      Pickle.save
     ]
   
   NN = {NewName}
   Goods = 
     [4711
      111111111111111111111111111111111111111111111111
      23.056
      {NewChunk Goods}
      f(a:    'an atom with blanks'
	NN:   BaseObject
	2:    Append
	true: fun {$ X Y} X+Y end
	cycle: Goods)
      {ByteString.make "bla"}
     ]
   
   Return =
   pickles(proc {$}
	      Tmp = {OS.tmpnam} LoadedGoods
	   in
	      % check nogoods
	      {All Nogoods fun {$ X} {TrySave X Tmp}==[X] end}=true

	      % check saving
	      {TrySave Goods Tmp} = nil
	      LoadedGoods = {Pickle.load Tmp}
	      %
	      cond Goods = LoadedGoods
	      then skip
	      else
		 raise
		    base(pickles('loaded goods mismatched:'
				 #Goods#LoadedGoods))
		 end
	      end

	      % check compressed save
	      {Pickle.saveCompressed Goods Tmp 9}
	      {Pickle.load Tmp} = Goods

	      {OS.unlink Tmp}
	   end
	   keys:[pickles])
end

