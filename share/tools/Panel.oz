%%%
%%% Authors:
%%%   Christian Schulte (schulte@dfki.de)
%%%
%%% Copyright:
%%%   Christian Schulte, 1997
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

declare

fun
\ifdef NEWCOMPILER
   instantiate
\endif
   {NewPanel IMPORT}
   \insert 'SP.env'
       = IMPORT.'SP'
   \insert 'OP.env'
       = IMPORT.'OP'
   \insert 'WP.env'
       = IMPORT.'WP'
in
   local
      \insert 'panel/main.oz'
      Panel = {New PanelClass init}
   in
      \insert 'Panel.env'
   end
end
