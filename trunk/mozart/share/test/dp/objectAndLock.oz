functor
import
   Remote(manager)
   Module(manager)
   OS(uName)
export
   Return
define
   Threads = 30
   Times = 50
   Sites = 3

   \insert TestMisc.oz
   MM = {New Module.manager init}
   TestMisc = {MM apply(url:'' FTestMisc $)}

   class Counter
      attr ctr

      meth init ctr <- 0 end

      meth checkMissedUpdates
	 case @ctr \= Threads * Times * Sites then
	    raise dp_objectAndLock_test_failed end
	 else skip end
      end
      
      meth updater(TimesLeft LastSeenNr Lock) New in
	 if TimesLeft == 0 then
	    skip
	 else
	    lock Lock then
	       New = @ctr+1 
	       ctr <- New
	    end
	    case LastSeenNr > New then
	       raise dp_objectAndLock_test_failed end
	    else skip end
	    Counter, updater(TimesLeft-1 New Lock)
	 end
      end
   end
   
   proc {Start} Managers in
      try
	 local
	    proc {Loop Ms Ctr Lock Ps}
	       case Ms
	       of M|Mr then Pr in
		  Ps = proc {$} {StartSite M Ctr Lock} end | Pr
		  {Loop Mr Ctr Lock Pr}
	       [] nil then
		  Ps = nil
	       end
	    end
	    Ctr = {New Counter init}
	    Lock = {NewLock}
	    Hosts Procs
	 in
	    {TestMisc.getHostNames Hosts}
	    {TestMisc.getRemoteManagers Sites Hosts Managers}
	    {Loop Managers Ctr Lock Procs}
	    {TestMisc.barrierSync Procs}
	    {Ctr checkMissedUpdates}
	 end
      catch X then
	 {TestMisc.gcAll Managers}
	 raise X end
      end
      {TestMisc.gcAll Managers}
      {TestMisc.listApply Managers close}      
   end      

   proc {StartSite RMan Ctr Lock} Error in
      {RMan apply(url:'' functor
			 define
			    proc {StartThreads Ctr Lock} 
			       List = {MakeList Threads}
			    in
			       {For 1 Threads 1
				proc {$ I}
				   {Nth List I
				    proc {$}
				       {Ctr updater(Times 0 Lock)}
				    end}
				end}
			       {BarrierSync List}
			    end
			    
			    proc {BarrierSync Ps}
			       proc {Conc Ps L}
				  case Ps of P|Pr then X Ls in
				     L = X|Ls
				     thread {P} X=unit end
				     {Conc Pr Ls}
				  else
				     L = nil
				  end
			       end
			       L
			    in
			       {Conc Ps L}
			       {List.forAll L proc {$ X} {Wait X} end}
			    end

			    proc {Start Ctr Lock Error} 
			       MemCell = {NewCell ok} in
			       try
				  {StartThreads Ctr Lock}
			       catch X then
				  {Assign MemCell X} 
			       end
			       Error = {Access MemCell}
			    end

			    {Start Ctr Lock Error}
			 end)}
      {TestMisc.raiseError Error}
   end

   Return = dp([object(['lock'(Start keys:[remote])])])
end

