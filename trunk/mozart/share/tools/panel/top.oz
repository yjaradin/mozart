%%%  Programming Systems Lab, DFKI Saarbruecken,
%%%  Stuhlsatzenhausweg 3, D-66123 Saarbruecken, Phone (+49) 681 302-5312
%%%  Author: Christian Schulte
%%%  Email: schulte@dfki.uni-sb.de
%%%  Last modified: $Date$ by $Author$
%%%  Version: $Revision$

local

   class UpdatePage from TkTools.note
      feat
	 options
	 top
      meth init(parent:P top:Top options:O text:T)
	 TkTools.note,tkInit(parent:P text:T)
	 self.options = O
	 self.top     = Top
      end
      meth toTop
	 {self.top update(false)}
      end
   end

   class ThreadPage from UpdatePage
      prop final
      attr
	 InfoVisible: false
	 PrevRuntime: ZeroTime

      meth setInfo(I)
	 InfoVisible <- I
      end
      meth update(What)
	 O   = self.options
	 OT  = O.threads
	 OP  = O.priorities
	 OR  = O.runtime
	 T   = {System.get threads}
	 P   = {System.get priorities}
	 R   = {System.get time}
	 PR  = @PrevRuntime
      in
	 case What==nosample then skip else
	    DiffUsed  = R.user + R.system - PR.user - PR.system
	    DiffTotal = R.total - PR.total
	 in
	    {OT.load    display([{IntToFloat T.runnable}])}
	    {OR.curLoad display([case DiffTotal==0 then 0.0 else
				    {IntToFloat DiffUsed} /
				    {IntToFloat DiffTotal}
				 end])}
	    PrevRuntime <- R
	 end
	 case What==sample then skip else
	    {OR.timebar     display(R)}
	    {OT.created     set(T.created)}
	    {OT.runnable    set(T.runnable)}
	    case @InfoVisible then
	       {OP.high        set(P.high)}
	       {OP.medium      set(P.medium)}
	    else skip
	    end
	    {OR.run         set(R.run)}
	    {OR.gc          set(R.gc)}
	    {OR.copy        set(R.copy)}
	    {OR.propagation set(R.propagate)}
	    {OR.load        set(R.load)}
	 end
      end
      meth clear
	 O  = self.options
	 OT = O.threads
	 OR = O.runtime
      in
	 {OR.timebar     clear}
	 {OT.created     clear}
	 {OT.load        clear}
	 {OR.run         clear}
	 {OR.gc          clear}
	 {OR.copy        clear}
	 {OR.propagation clear}
	 {OR.load        clear}
	 {OR.curLoad     clear}
	 PrevRuntime <- {System.get time}
      end
      meth toggleInfo
	 O = self.options
      in
	 {Tk.send case @InfoVisible then pack(forget O.priorities.frame)
		  else pack(O.priorities.frame
			    after:O.threads.frame side:top fill:x padx:3)
		  end}
	 InfoVisible <- {Not @InfoVisible}
      end
   end
   
   class MemoryPage
      from UpdatePage
      prop final
      attr InfoVisible:  false
      
      meth setInfo(I)
	 InfoVisible <- I
      end
      meth update(What)
	 O   = self.options
	 OG  = O.gc
	 OU  = O.usage
	 G   = {System.get gc}
      in
	 case What==nosample then skip else
	    {OU.load       display([{IntToFloat G.threshold} / MegaByteF
				    {IntToFloat G.size} / MegaByteF
				    {IntToFloat G.active} / MegaByteF])}
	 end
	 case What==sample then skip else
	    {OU.active     set(G.active div KiloByteI)}
	    {OU.size       set(G.size div KiloByteI)}
	    {OU.threshold  set(G.threshold div KiloByteI)}
	    case @InfoVisible then
	       OP  = O.parameter
	       OU  = O.usage
	    in
	       {OP.minSize    set(G.min div MegaByteI)}
	       {OP.maxSize    set(G.max div MegaByteI)}
	       {OP.free       set(G.free)}
	       {OP.tolerance  set(G.tolerance)}
	       {OG.active     set(G.on)}
	    else
	       OP  = O.showParameter
	    in
	       {OP.minSize    set(G.min div MegaByteI)}
	       {OP.maxSize    set(G.max div MegaByteI)}
	    end
	 end
      end
      meth clear
	 {self.options.usage.load clear}
      end
      meth toggleInfo
	 O = self.options
      in
	 {Tk.batch case @InfoVisible then
		      [pack(forget O.parameter.frame O.gc.frame)
		       pack(O.showParameter.frame 
			    after:O.usage.frame side:top fill:x padx:3)]
		   else
		      [pack(forget O.showParameter)
		       pack(O.parameter.frame O.gc.frame
			    after:O.usage.frame side:top fill:x padx:3)]
		   end}
	 InfoVisible <- {Not @InfoVisible}
	 MemoryPage,update(nosample)
      end
   end

   class PsPage
      from UpdatePage
      prop final

      meth update(...)
	 O  = self.options
	 OS = O.spaces
	 OF = O.fd
	 S = {System.get spaces}
	 F = {System.get fd}
      in
	 {OS.created   set(S.created)}
	 {OS.cloned    set(S.cloned)}
	 {OS.committed set(S.committed)}
	 {OS.failed    set(S.failed)}
	 {OS.succeeded set(S.succeeded)}
	 {OF.propc     set(F.propagators)}
	 {OF.propi     set(F.invoked)}
	 {OF.var       set(F.variables)}
      end
      meth clear
	 O  = self.options
	 OS = O.spaces
	 OF = O.fd
      in
	 {OS.created   clear}
	 {OS.cloned    clear}
	 {OS.committed clear}
	 {OS.failed    clear}
	 {OS.succeeded clear}
	 {OF.propc     clear}
	 {OF.propi     clear}
	 {OF.var       clear}
      end

   end
   
   class OpiPage
      from UpdatePage
      prop final

      meth update(...)
	 O  = self.options
	 OE = O.errors
	 OP = O.output
	 OM = O.messages
	 E = {System.get errors}
	 P = {System.get print}
	 M = {System.get messages}
      in
	 {OE.'thread' set(E.'thread')}
	 {OE.width    set(E.width)}
	 {OE.depth    set(E.depth)}
	 {OE.location set(E.location)}
	 {OE.hints    set(E.hints)}
	 {OP.width    set(P.width)}
	 {OP.depth    set(P.depth)}
	 {OM.gc       set(M.gc)}
	 {OM.time     set(M.idle)}
      end

   end

   fun {ProjectSnd X Y} Y end
   
in
   
   class PanelTop
      from Tk.toplevel DialogClass
      prop
	 locking
	 final
      feat
	 manager options
	 notebook
	 menu
	 threads memory opi ps
      attr
	 UpdateTime:    DefaultUpdateTime
	 HistoryRange:  DefaultHistoryRange
	 RequireMouse:  true
	 MouseInside:   true
	 DelayStamp:    0
	 InfoVisible:   false
      
      meth init(manager:Manager options:O)
	 lock
	    Config = {Dictionary.get O config}
	    Tk.toplevel,tkInit(title:              TitleName
			       highlightthickness: 0
			       withdraw:           true)
	    {Tk.batch [wm(iconname   self TitleName)
		       wm(iconbitmap self BitMap)
		       wm(resizable self 0 0)]}
	    EventFrame = {New Tk.frame tkInit(parent:             self
					      highlightthickness: 0)}
	    Menu  = {TkTools.menubar EventFrame self
		     [menubutton(text:    ' Panel '
				 feature: panel
				 font:    BoldFont
				 menu:
		        [command(label:   'About...'
				 action:  self # about
				 font:    BoldFont
				 feature: about)
			 separator
			 command(label:   'Reset'
				 key:     ctrl(r)
				 font:    BoldFont
				 action:  self # clear)
			 separator
			 command(label:   'Save Parameters...'
				 action:  self # save
				 font:    BoldFont
				 feature: save)
			 command(label:   'Shutdown System...'
				 action:  self # shutdown
				 font:    BoldFont
				 feature: shutdown)
			 separator
			 command(label:   'Close'
				 action:  self # tkClose
				 font:    BoldFont
				 key:     ctrl(x))])
		      menubutton(text:    ' Options '
				 font:    BoldFont
				 feature: options
				 menu:
		         [checkbutton(label: 'Configure'
				      font:    BoldFont
				      variable: {New Tk.variable
						 tkInit(Config)}
				      action: self # toggleInfo)
			  separator
			  command(label:  'Update...'
				  font:    BoldFont
				  action:  self # optionUpdate
				  feature: update)
			  command(label:  'History...'
				  font:    BoldFont
				  action:  self # optionHistory
				  feature: history)])
		     ]
		     nil}
	    {Menu.panel.menu   tk(conf tearoff:false)}
	    {Menu.options.menu tk(conf tearoff:false)}
	    Frame = {New Tk.frame tkInit(parent: EventFrame
					 highlightthickness: 0
					 bd:                 4)}
	    Book  = {New TkTools.notebook tkInit(parent:Frame
						 font:BoldFont)}
	    Threads =
	    {MakePage ThreadPage 'Threads' Book self true
	     [frame(text:    'Runtime'
		    feature: runtime
		    left:
		       [time(text:    'Run:'
			     feature: run
			     color:   TimeColors.run
			     stipple: TimeStipple.run)
			time(text:    'Garbage Collection:'
			     feature: gc
			     color:   TimeColors.gc
			     stipple: TimeStipple.gc)
			time(text:    'Copy:'
			     feature: copy
			     color:   TimeColors.copy
			     stipple: TimeStipple.copy)
			time(text:    'Propagation:'
			     feature: propagation
			     color:   TimeColors.'prop'
			     stipple: TimeStipple.'prop')
			time(text:    'Load:'
			     feature: load
			     color:   TimeColors.load
			     stipple: TimeStipple.load)]
		    right:
		       [load(feature: curLoad
			     colors:  [CurLoadColor]
			     stipple: ['']
			     maxy:    1.0
			     miny:    1.0)
			timebar(feature: timebar)])
	      frame(text:    'Threads'
		    feature: threads
		    left:
		       [number(text:    'Created:'
			       feature: created)
			number(text:    'Runnable:'
			       feature: runnable
			       color:   RunnableColor
			       stipple: RunnableStipple)]
		    right:
		       [load(feature: load
			     colors:  [RunnableColor]
			     stipple: [RunnableStipple])])
	      frame(text:    'Priorities'
		    feature: priorities
		    pack:    Config
		    left:
		       [entry(text:    'High / Medium:'
			      feature: high
			      max:     100
			      init:    {System.get priorities}.high
			      action:  proc {$ N}
					  {System.set priorities(high:N)}
				       end)
			entry(text:    'Medium / Low:'
			      feature: medium
			      max:     100
			      init:    {System.get priorities}.medium
			      action:  proc {$ N}
					  {System.set priorities(medium:N)}
				       end)]
		    right:
		       [button(text:    'Default'
			       feature: default
			       action:  proc {$}
					   {System.set priorities(high:   10
								  medium: 10)}
					   {self update(false)}
					end)])]}
	    Memory =
	    {MakePage MemoryPage 'Memory' Book self true
	     [frame(text:    'Heap Usage'
		    feature: usage
		    left:
		       [size(text:    'Threshold:'
			     feature: threshold
			     color:   ThresholdColor
			     stipple: ThresholdStipple)
			size(text:    'Size:'
			     feature: size
			     color:   SizeColor
			     stipple: SizeStipple)
			size(text:    'Active Size:'
			     feature: active
			     color:   ActiveColor
			     stipple: ActiveStipple)]
		    right:
		       [load(feature: load
			     colors:  [ThresholdColor   SizeColor
				       ActiveColor]
			     stipple: [ThresholdStipple SizeStipple
				       ActiveStipple]
			     dim:     'MB')])
	      frame(text:    'Heap Parameters'
		    feature: parameter
		    pack:    Config
		    left:
		       [entry(text:    'Minimal Size:'
			      feature: minSize
			      min:     1
			      max:     1024
			      dim:     'MB'
			      init:    {System.get gc}.min div MegaByteI
			      action:  proc {$ N}
					  S = Memory.options.parameter.maxSize
				       in
					  case {S get($)}<N then
					     {S set(N)}
					     {System.set gc(min:N * MegaByteI
							    max:N * MegaByteI)}
					  else
					     {System.set gc(min:N * MegaByteI)}
					  end
				       end)
			entry(text:    'Free:'
			      feature: free
			      max:     100
			      init:    {System.get gc}.free
			      side:    right
			      action:  proc {$ N}
					  {System.set gc(free: N)}
				       end
			      dim:     '%')
			entry(text:    'Maximal Size:'
			      feature: maxSize
			      min:     1
			      max:     1024
			      dim:     'MB'
			      init:    local MS={System.get gc}.max in
					  case MS=<0 then 1024
					  else MS div MegaByteI
					  end
				       end
			      action:  proc {$ N}
					  S = Memory.options.parameter.minSize
				       in
					  case {S get($)}>N then
					     {S set(N)}
					     {System.set gc(min:N * MegaByteI
							    max:N * MegaByteI)}
					  else
					     {System.set gc(max:N * MegaByteI)}
					  end
				       end)
			entry(text:    'Tolerance:'
			      feature: tolerance
			      max:     100
			      dim:     '%'
			      side:    right
			      init:    {System.get gc}.tolerance
			      action:  proc {$ N}
					  {System.set gc(tolerance: N)}
				       end)]
	            right:
		       [button(text:   'Small'
			       feature: small	   
			       action:  proc {$}
					   {System.set
					    gc(max:       4 * MegaByteI
					       min:       1 * MegaByteI
					       free:      75
					       tolerance: 20)}
					   {self update(false)}
					end)
			button(text:    'Medium'
			       feature: medium
			       action:  proc {$}
					   {System.set
					    gc(max:       16 * MegaByteI
					       min:       2  * MegaByteI
					       free:      80
					       tolerance: 15)}
					   {self update(false)}
					end)
			button(text:    'Large'
			       feature: large
			       action:  proc {$}
					   {System.set
					    gc(max:       64 * MegaByteI
					       min:       8 * MegaByteI
					       free:      90
					       tolerance: 10)}
					   {self update(false)}
					end)])
	      frame(text:    'Heap Parameters'
		    feature: showParameter
		    pack:    {Not Config}
		    left:
		       [size(text:    'Maximal Size:'
			     feature: maxSize
			     dim:     'MB')
			size(text:    'Minimal Size:'
			     feature: minSize
			     dim:     'MB')]
		    right: nil)
	      frame(text:    'Garbage Collector'
		    feature: gc
		    pack:    Config
		    left:
		       [checkbutton(text:   'Active'
				    feature: active
				    state:  {System.get gc}.on
				    action: proc {$ OnOff}
					       {System.set gc(on:OnOff)}
					    end)]
		    right:   [button(text: 'Invoke'
				     feature: invoke
				     action: proc {$}
						{System.gcDo}
					     end)])]}
	     PS =
	     {MakePage PsPage 'Problem Solving' Book self true
	      [frame(text:    'Finite Domain Constraints'
		     feature: fd
		     left:    [number(text: 'Variables Created:'
				      feature: var)
			       number(text:    'Propagators Created:'
				      feature: propc)
			       number(text:    'Propagators Invoked:'
				      feature: propi)]
		     right:   nil)
	       frame(text:    'Spaces'
		     feature: spaces
		     left:    [number(text:    'Created:'
				      feature: created)
			       number(text:    'Cloned:'
				      feature: cloned)
			       number(text:    'Committed:'
				      feature: committed)
			       number(text:    'Failed:'
				      feature: failed)
			       number(text:    'Succeeded:'
				      feature: succeeded)]
		     right:   nil)]}
	     OPI =
	     {MakePage OpiPage 'Programming Interface' Book self Config
	      [frame(text:    'Status Messages'
		     feature: messages
		     left:
			[checkbutton(text:    'Idle'
				     feature: time
				     state:  {System.get messages}.idle
				     action: proc {$ B}
						{System.set messages(idle:B)}
					     end)
			 checkbutton(text:    'Garbage Collection'
				     feature: gc
				     state:  {System.get messages}.gc
				     action: proc {$ B}
						{System.set messages(gc:B)}
					     end)]
		     right:
			[button(text:    'Default'
				feature: default
				action:  proc {$}
					    {System.set messages(idle: false
								 gc:   false)}
					    {self update(false)}
					 end)])
	       frame(text:    'Output'
		     feature: output
		     left:
			[entry(text:    'Maximal Depth:'
			       feature: depth
			       action:  proc {$ N}
					   {System.set print(depth: N)}
					end)
			 entry(text:    'Maximal Width:'
			       feature: width
			       action:  proc {$ N}
					   {System.set print(width: N)}
					end)]
		     right:
			[button(text:    'Default'
				feature: default
				action:  proc {$}
					    {System.set print(width: 10
							      depth: 2)}
					    {self update(false)}
					 end)])
	       frame(text:    'Errors'
		     feature: errors
		     left:
			[checkbutton(text:    'Show Location'
				     feature: location
				     state:   {System.get errors}.location
				     action:  proc {$ B}
						 {System.set
						  errors(location:B)}
					      end)
			 checkbutton(text:    'Show Hints'
				     feature: hints
				     state:   {System.get errors}.hints
				     action:  proc {$ B}
						 {System.set errors(hints:B)}
					      end)
			 entry(text:    'Maximal Depth:'
			       feature: depth
			       action:  proc {$ N}
					   {System.set errors(depth: N)}
					end)
			 entry(text:    'Maximal Tasks:'
			       feature: 'thread'
			       side:    right
			       action:  proc {$ N}
					   {System.set errors('thread': N)}
					end)
			 entry(text:    'Maximal Width:'
			       feature: width
			       action:  proc {$ N}
					   {System.set errors(width: N)}
					end)]
		     right:
			[button(text:    'Default'
				feature: default
				action:  proc {$}
					    {System.set
					     errors('thread': 10
					            location: true
					            hints:    true
						    width:    10
						    depth:    2)}
					    {self update(false)}
					 end)])]}
         in
	    {Tk.batch [pack(Menu side:top fill:x)
		       pack(Book)
		       pack(Frame side:bottom)
		       pack(EventFrame)]}
	    self.manager  = Manager
	    self.threads  = Threads
	    self.memory   = Memory
	    self.opi      = OPI
	    self.ps       = PS
	    self.notebook = Book
	    self.menu     = Menu
	    {EventFrame tkBind(event:'<Enter>' action:self # enter)}
	    {EventFrame tkBind(event:'<Leave>' action:self # leave)}
	    PanelTop, tkWM(deiconify)
	    self.options = O
	    RequireMouse <- {Dictionary.get O mouse}
	    UpdateTime   <- {Dictionary.get O time}
	    HistoryRange <- {Dictionary.get O history}
	    InfoVisible  <- Config
	    {Threads setInfo(Config)}
	    {Memory  setInfo(Config)}
	 end
	 PanelTop, delay(0)
      end

      meth update(Regular)
	 lock
	    TopNote = {self.notebook getTop($)}
	    Threads = self.threads
	    Memory  = self.memory
	 in
	    case Regular then
	       case TopNote
	       of !Threads then
		  {Threads update(both)}
		  {Memory  update(sample)}
	       [] !Memory  then
		  {Threads update(sample)}
		  {Memory  update(both)}
	       else
		  {Threads update(sample)}
		  {Memory  update(sample)}
		  {TopNote update}
	       end
	    else {TopNote update(nosample)}
	    end
	 end
      end

      meth shutdown
	 {self.menu.panel.shutdown tk(entryconf state:disabled)}
	 case DialogClass, shutdown($) then {System.exit 0}
	 else skip
	 end
	 {self.menu.panel.shutdown tk(entryconf state:normal)}
      end

      meth save
	 {self.menu.panel.save tk(entryconf state:disabled)}
	 case {Tk.return
	       tk_getSaveFile(filetypes:  q(q('Oz Files'  q('.oz'))
					    q('All Files' '*'))
			      parent:     self
			      title:      TitleName#': Save Parameters')}
	 of nil then skip
	 elseof S then
	    try
	       F = {New Open.file init(name:S flags:[write create truncate])}
	    in
	       {F write(vs:('%%\n' #
			    '%% System parameters (Created by Oz Panel)\n' #
			    '%%\n'))}
	       {ForAll
		[priorities(high:unit medium:unit)
		 gc(min:unit max:unit free:unit tolerance:unit on:unit)
		 messages(idle:unit gc:unit)
		 print(depth:unit width:unit)
		 errors(depth:unit hints:unit location:unit 'thread':unit)]
		proc {$ SS}
		   {F write(vs:('{System.set ' #
				  {System.valueToVirtualString
				   {Record.zip SS {System.get {Label SS}}
				    ProjectSnd} 100 100} #
				  '}\n'))}
		end}
	       {F close}
	    catch system(os(_ _ T) ...) then
	       {Wait {New TkTools.error
		      tkInit(master: self
			     text:   'Error in writing file: '#T)}.tkClosed}
	    end
	 end
	 {self.menu.panel.save tk(entryconf state:normal)}
      end

      meth about
	 {self.menu.panel.about tk(entryconf state:disabled)}
	 DialogClass, about
	 {self.menu.panel.about tk(entryconf state:normal)}
      end

      meth toggleInfo
	 lock
	    InfoVisible <- {Not @InfoVisible}
	    {Dictionary.put self.options config @InfoVisible}
	    case @InfoVisible then {self.notebook add(self.opi)}
	    else {self.notebook remove(self.opi)}
	    end
	    {self.threads toggleInfo}
	    {self.memory  toggleInfo}
	 end
      end
      
      meth delay(ODS)
	 DS UT
      in
	 lock
	    DS = @DelayStamp
	    UT = @UpdateTime
	 end
	 case DS==ODS then {self update(true)} {Delay UT} {self delay(ODS)}
	 else skip
	 end
      end

      meth stop
	 lock
	    DelayStamp <- @DelayStamp + 1
	 end
      end
      
      meth enter
	 {Thread.setThisPriority high}
	 case
	    lock
	       MouseInside <- true
	       case @RequireMouse then
		  PanelTop,stop
		  @DelayStamp
	       else ~1
	       end
	    end
	 of ~1 then skip
	 elseof DS then {self delay(DS)}
	 end
      end

      meth leave
	 lock
	    MouseInside <- false
	    case @RequireMouse then PanelTop,stop
	    else skip
	    end
	 end
      end

      meth setSlice
	 S = (LoadWidth * @UpdateTime) div @HistoryRange
	 STO = self.threads.options
      in
	 {self.memory.options.usage.load slice(S)}
	 {STO.threads.load               slice(S)}
	 {STO.runtime.curLoad            slice(S)}
      end

      meth updateAfterOption
	 {Thread.setThisPriority high}
	 case
	    lock
	       O = self.options
	       H = {Dictionary.get O history}
	       M = {Dictionary.get O mouse}
	       T = {Dictionary.get O time}
	    in
	       case {Dictionary.get O config}==@InfoVisible then skip else
		  PanelTop, toggleInfo
	       end
	       case H==@HistoryRange then skip else
		  HistoryRange <- H
		  PanelTop,setSlice
	       end
	       {Max case @RequireMouse==M then ~1
		    else
		       RequireMouse <- M
		       case M then
			  case @MouseInside then @DelayStamp
			  else PanelTop,stop ~1
			  end
		       else PanelTop,stop @DelayStamp
		       end
		    end
	            case @UpdateTime==T then ~1
		    else
		       UpdateTime <- T
		       PanelTop,stop
		       PanelTop,setSlice
		       @DelayStamp
		    end}
	    end
	 of ~1 then skip
	 elseof DS then {self delay(DS)}
	 end
      end
      
      meth optionUpdate
	 {self.menu.options.update tk(entryconf state:disabled)}
	 DialogClass, update
	 {self.menu.options.update tk(entryconf state:normal)}
	 PanelTop, updateAfterOption
      end

      meth optionHistory
	 {self.menu.options.history tk(entryconf state:disabled)}
	 DialogClass, history
	 {self.menu.options.history tk(entryconf state:normal)}
	 PanelTop, updateAfterOption
      end

      meth clear
	 lock
	    {self.threads clear}
	    {self.memory  clear}
	    {self.ps      clear}
	 end
      end
      
      meth tkClose
	 lock
	    {self.manager PanelTopClosed}
	    Tk.toplevel, tkClose
	    {Wait _}
	 end
      end
      
   end


end
