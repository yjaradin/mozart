%%%
%%% Author:
%%%   Leif Kornstaedt <kornstae@ps.uni-sb.de>
%%%
%%% Copyright:
%%%   Leif Kornstaedt, 1997
%%%
%%% Last change:
%%%   $Date$ by $Author$
%%%   $Revision$
%%%
%%% This file is part of Mozart, an implementation of Oz 3:
%%%    $MOZARTURL$
%%%
%%% See the file "LICENSE" or
%%%    $LICENSEURL$
%%% for information on usage and redistribution
%%% of this file, and for a DISCLAIMER OF ALL
%%% WARRANTIES.
%%%

local
   % Misc
   PrintNameToVirtualString
   IsPrintName
   NameVariable
   NewNamedName
   IsBuiltin
   GetBuiltinName
   GenerateAbstractionTableID
   ConcatenateAtomAndInt

   % Builtins
   GetBuiltinInfo

   % CoreLanguage
   FlattenSequence
   Core
in
   \insert Misc
   \insert Builtins

   local
      ImAConstruction       = {NewName}
      ImAValueNode          = {NewName}
      ImAVariableOccurrence = {NewName}
      ImAToken              = {NewName}

      % Annotate
      Annotate

      % StaticAnalysis
      SA

      % CodeGen
      CodeGen
   in
      \insert Annotate

      local
	 % POTypes
	 OzTypes
	 TypeConstants
	 OzValueToType
      in
	 \insert POTypes
	 \insert StaticAnalysis
      end

      local
	 % RegSet
	 RegSet

	 % CodeEmitter
	 Emitter

	 % CodeStore
	 Continuations
	 CodeStore
      in
	 \insert RegSet
	 \insert CodeEmitter
	 \insert CodeStore
	 \insert CodeGen
      end

      \insert CoreLanguage
   end

   local
      % Version
      OZVERSION
      DATE

      % FormatStrings
      FormatStringToVirtualString

      % Reporter
      Reporter

      % ParseOz
      ParseOzFile
      ParseOzVirtualString

      % Unnester
      JoinQueries
      MakeExpressionQuery
      UnnestQuery

      % Interface
      Interface

      % Assembler
      Assemble
   in
      \insert compiler-Version
      \insert FormatStrings
      \insert Reporter
      \insert ParseOz
      local
	 % TupleSyntax
	 CoordinatesOf
	 VarListSub
	 GetPatternVariablesStatement
	 GetPatternVariablesExpression
	 UniqueVariables
	 PrivateAttrFeat
	 PrivateMeth

	 % BindingAnalysis
	 BindingAnalysis

	 % UnnestFD
	 MakeFdCompareStatement
	 MakeFdCompareExpression
	 MakeCondis
      in
	 \insert TupleSyntax
	 \insert BindingAnalysis
	 \insert UnnestFD
	 \insert Unnester
      end
      \insert Interface
      \insert Assembler
      \insert CompilerClass
      \insert GenericInterface
      \insert QuietInterface
   end
end
