require 'multi-tk.rb'

th = Thread.new{Tk.mainloop}

TkLabel.new(:text=>'this is a primary master').pack

ip1 = MultiTkIp.new_slave(:safe=>1)
ip2 = MultiTkIp.new_slave(:safe=>2)

cmd = proc{|s|
  require 'tk'

  TkButton.new(:text=>'b1: p self', :command=>proc{p self}).pack(:fill=>:x)
  sleep s
  TkButton.new(:text=>'b2: p $SAFE', :command=>proc{p $SAFE}).pack(:fill=>:x)
  sleep s
  TkButton.new(:text=>'b3: p MultiTkIp.ip_name',
               :command=>proc{p MultiTkIp.ip_name}).pack(:fill=>:x)
  sleep s
  TkButton.new(:text=>'EXIT', :command=>proc{exit}).pack(:fill=>:x)

  Tk.mainloop
}

Thread.new{ip1.eval_proc(cmd, 1.1)}
Thread.new{ip2.eval_proc(cmd, 0.3)}
cmd.call(0.7)

th.join
