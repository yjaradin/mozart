%%%
%%% Authors:
%%%   Martin Henz (henz@iscs.nus.edu.sg)
%%%   Martin Mueller (mmueller@ps.uni-sb.de)
%%%   Christian Schulte (schulte@dfki.de)
%%%
%%% Copyright:
%%%   Martin Henz, 1997
%%%   Martin Mueller, 1997
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


local

   fun {IsFeature X}
      {IsInt X} orelse {IsLiteral X}
   end

   fun {IsOrdered X}
      {IsNumber X} orelse {IsAtom X}
   end

   fun {IsUnary X}
      {IsObject X} orelse
      ({IsProcedure X} andthen {ProcedureArity X}==1)
   end

   fun {IsPair X}
      case X of _#_ then true else false end
   end

   fun {IsListOf Xs P}
      case Xs of nil then true
      [] X|Xr then {P X} andthen {IsListOf Xr P}
      else false end
   end

   fun {IsPairOf X P1 P2}
      {IsPair X} andthen {P1 X.1} andthen {P2 X.2}
   end

   fun {IsRecordOf R Fs P}
      case Fs of nil then true
      [] F|Fr then {P R.F} andthen {IsRecordOf R Fr  P}
      end
   end

   fun {IsPropList Xs}
      {IsListOf Xs fun {$ X} {IsPair X} andthen {IsLiteral X.1} end}
   end

   local
      fun {IsComplexDomSpec Xs}
	 {IsInt Xs} orelse
	 {IsPairOf Xs IsInt IsInt} orelse
	 {IsListOf Xs fun {$ X}
			 {IsInt X} orelse {IsPairOf X IsInt IsInt}
		      end}
      end
   in
      fun {IsDomainSpec Xs}
	 case Xs
	 of compl(Ys) then {IsComplexDomSpec Ys}
	 else {IsComplexDomSpec Xs} end
      end
   end

in
   
   functor prop once

   import
      FDB.is
         from 'x-oz://boot/FDB'

      FSB.{isValueB isVarB}
         from 'x-oz://boot/FSB'

      System.{printName}	    

   export
      is:      Is
      ask:     Ask
      
   body

      IsFDIntC  = FDB.is
      IsFSet    = FSB.isValueB
      IsFSetC   = FSB.isVarB

      fun {IsFDVector X}
	 case {IsRecord X}
	 then case {Label X}=='|'
	      then {IsListOf X IsFDIntC}
	      else {IsRecordOf X {Arity X} IsFDIntC}
	      end
	 else false end
      end

      Is = is(array:               IsArray
	      atom:                IsAtom
	      bitArray:            IsBitArray
	      bool:                IsBool
	      char:                IsChar
	      chunk:               IsChunk
	      dictionary:          IsDictionary
	      domainSpec:          IsDomainSpec
	      int:                 IsInt
	      fdIntC:              IsFDIntC
	      fdVector:            IsFDVector
	      feature:             IsFeature
	      float:               IsFloat
	      fset:                IsFSet
	      fsetC:               IsFSetC
	      foreignPointer:      IsForeignPointer
	      list:                IsList
	      'lock':              IsLock
	      literal:             IsLiteral
	      name:                IsName
	      number:              IsNumber
	      object:              IsObject
	      ordered:             IsOrdered
	      pair:                IsPair
	      port:                IsPort
	      procedure:           IsProcedure
	      propertyList:        IsPropList
	      record:              IsRecord
	      recordC:             IsRecordC
	      space:               IsSpace
	      string:              IsString
	      unary:               IsUnary
	      'unit':              IsUnit
	      virtualString:       IsVirtualString
	      'class':             IsClass
	      'thread':            IsThread
	     )

      local
	 fun {GenericAsk TypeTest TypeName}
	    proc {$ X}
	       case {TypeTest X} then skip else
		  {`RaiseError` kernel(type {System.printName TypeTest}
				       [X]
				       TypeName
				       1
				       'Type.ask')}
	       end
	    end
	 end
      in
	 Ask = ask(generic:           GenericAsk
		   array:             {GenericAsk IsArray array}
		   atom:              {GenericAsk IsAtom atom}
		   bitArray:          {GenericAsk IsBitArray bitArray}
		   bool:              {GenericAsk IsBool bool}
		   char:              {GenericAsk IsChar char}
		   chunk:             {GenericAsk IsChunk chunk}
		   dictionary:        {GenericAsk IsDictionary dictionary}
		   domainSpec:        {GenericAsk IsDomainSpec domainSpec}
		   int:               {GenericAsk IsInt int}
		   fdIntC:            {GenericAsk IsFDIntC fd}
		   fdVector:          {GenericAsk IsFDVector fdVector}
		   feature:           {GenericAsk IsFeature feature}
		   float:             {GenericAsk IsFloat float}
		   foreignPointer:    {GenericAsk IsForeignPointer
				       foreignPointer}
		   list:              {GenericAsk IsList list}
		   literal:           {GenericAsk IsLiteral literal}
		   'lock':            {GenericAsk IsLock 'lock'}
		   name:              {GenericAsk IsName name}
		   number:            {GenericAsk IsNumber number}
		   object:            {GenericAsk IsObject object}
		   ordered:           {GenericAsk IsOrdered ordered}
		   pair:              {GenericAsk IsPair pair}
		   port:              {GenericAsk IsPort port}
		   procedure:         {GenericAsk IsProcedure procedure}
		   propertyList:      {GenericAsk IsPropList propertyList}
		   record:            {GenericAsk IsRecord record}
		   recordC:           {GenericAsk IsRecordC recordC}
		   space:             {GenericAsk IsSpace space}
		   string:            {GenericAsk IsString string}
		   unary:             {GenericAsk IsUnary unary}
		   'unit':            {GenericAsk IsUnit 'unit'}
		   virtualString:     {GenericAsk IsVirtualString
				       virtualString}
		   'class':           {GenericAsk IsClass 'class'}
		   'thread':          {GenericAsk IsThread 'thread'}
		  )
      end
      
   end

end
