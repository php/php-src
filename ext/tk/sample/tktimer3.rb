#!/usr/bin/env ruby
# This script is a re-implementation of tktimer.rb with TkTimer(TkAfter) class.

require "tk"

# new notation :
#   * symbols are acceptable as keys or values of the option hash
#   * the parent widget can be given by :parent key on the option hash
root = TkRoot.new(:title=>'timer sample')
label = TkLabel.new(:parent=>root, :relief=>:raised, :width=>10) \
               .pack(:side=>:bottom, :fill=>:both)

# define the procedure repeated by the TkTimer object
tick = proc{|aobj| #<== TkTimer object
  cnt = aobj.return_value + 5 # return_value keeps a result of the last proc
  label.text format("%d.%02d", *(cnt.divmod(100)))
  cnt #==> return value is kept by TkTimer object
      #    (so, can be send to the next repeat-proc)
}

timer = TkTimer.new(50, -1, tick).start(0, proc{ label.text('0.00'); 0 })
        # ==> repeat-interval : (about) 50 ms,
        #     repeat : infinite (-1) times,
        #     repeat-procedure : tick (only one, in this case)
        #
        # ==> wait-before-call-init-proc : 0 ms,
        #     init_proc : proc{ label.text('0.00'); 0 }
        #
        # (0ms)-> init_proc ->(50ms)-> tick ->(50ms)-> tick ->....

b_start = TkButton.new(:text=>'Start', :state=>:disabled) {
  pack(:side=>:left, :fill=>:both, :expand=>true)
}

b_stop  = TkButton.new(:text=>'Stop', :state=>:normal) {
  pack('side'=>'left', 'fill'=>'both', 'expand'=>'yes')
}

b_start.command {
  timer.continue
  b_stop.state(:normal)
  b_start.state(:disabled)
}

b_stop.command {
  timer.stop
  b_start.state(:normal)
  b_stop.state(:disabled)
}

TkButton.new(:text=>'Reset', :state=>:normal) {
  command { timer.reset }
  pack(:side=>:right, :fill=>:both, :expand=>:yes)
}

ev_quit = TkVirtualEvent.new('Control-c', 'Control-q')
Tk.root.bind(ev_quit, proc{Tk.exit}).focus

Tk.mainloop
