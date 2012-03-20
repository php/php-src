#!/usr/bin/env ruby
# This script is a sample of MultiTkIp class

require "multi-tk"

# create slave interpreters
trusted_slave = MultiTkIp.new_slave
safe_slave1   = MultiTkIp.new_safeTk
safe_slave2   = MultiTkIp.new_safeTk('fill'=>:none, 'expand'=>false)
#safe_slave2   = MultiTkIp.new_safeTk('fill'=>:none)
#safe_slave2   = MultiTkIp.new_safeTk('expand'=>false)

cmd = Proc.new{|txt|
  #####################
  ## from TkTimer2.rb

  if TkCore::INTERP.safe?
    # safeTk doesn't have permission to call 'wm' command
  else
    root = TkRoot.new(:title=>'timer sample')
  end
  label = TkLabel.new(:parent=>root, :relief=>:raised, :width=>10) \
                  .pack(:side=>:bottom, :fill=>:both)

  tick = proc{|aobj|
    cnt = aobj.return_value + 5
    label.text format("%d.%02d", *(cnt.divmod(100)))
    cnt
  }

  timer = TkTimer.new(50, -1, tick).start(0, proc{ label.text('0.00'); 0 })

=begin
  TkButton.new(:text=>'Start') {
    command proc{ timer.continue unless timer.running? }
    pack(:side=>:left, :fill=>:both, :expand=>true)
  }
  TkButton.new(:text=>'Restart') {
    command proc{ timer.restart(0, proc{ label.text('0.00'); 0 }) }
    pack('side'=>'right','fill'=>'both','expand'=>'yes')
  }
  TkButton.new(:text=>'Stop') {
    command proc{ timer.stop if timer.running? }
    pack('side'=>'right','fill'=>'both','expand'=>'yes')
  }
=end
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
    pack('side'=>'right', 'fill'=>'both', 'expand'=>'yes')
  }

  ev_quit = TkVirtualEvent.new('Control-c', 'Control-q')
  Tk.root.bind(ev_quit, proc{Tk.exit}).focus
}

# call on the default master interpreter
trusted_slave.eval_proc(cmd, 'trusted')  # label -> .w00012
safe_slave1.eval_proc(cmd, 'safe1')      # label -> .w00016
safe_slave2.eval_proc(cmd, 'safe2')      # label -> .w00020
cmd.call('master')                       # label -> .w00024

#second_master = MultiTkIp.new(&cmd)
#second_master = MultiTkIp.new(:safe=>2){p [:second_master, $SAFE]}

TkTimer.new(2000, -1, proc{p ['safe1', safe_slave1.deleted?]}).start
TkTimer.new(2000, -1, proc{p ['safe2', safe_slave2.deleted?]}).start
TkTimer.new(2000, -1, proc{p ['trusted', trusted_slave.deleted?]}).start

TkTimer.new(5000, 1,
            proc{
              safe_slave1.eval_proc{Tk.root.destroy}
              safe_slave1.delete
              print "*** The safe_slave1 is deleted by the timer.\n"
            }).start

TkTimer.new(10000, 1,
            proc{
              trusted_slave.eval_proc{Tk.root.destroy}
              trusted_slave.delete
              print "*** The trusted_slave is deleted by the timer.\n"
            }).start

Tk.mainloop
