#!/usr/bin/env ruby

require 'multi-tk'

TkMessage.new(:text => <<EOM).pack
This is a sample of the safe-Tk slave interpreter. \
On the slave interpreter, 'tkoptdb.rb' demo is running.
( NOTE:: a safe-Tk interpreter can't read options \
from a file. Options are given by the master interpreter \
in this script. )
The window shown this message is a root widget of \
the default master interpreter. The other window \
is a toplevel widget of the master interpreter, and it \
has a container frame of the safe-Tk slave interpreter.
'exit' on the slave interpreter exits the slave only. \
You can also delete the slave by the button on the toplevel widget.
EOM

if ENV['LANG'] =~ /^ja/
  # read Japanese resource
  ent = TkOptionDB.read_entries(File.expand_path('resource.ja',
                                                 File.dirname(__FILE__)),
                                'utf-8')
else
  # read English resource
  ent = TkOptionDB.read_entries(File.expand_path('resource.en',
                                                File.dirname(__FILE__)))
end

file = File.expand_path('tkoptdb.rb', File.dirname(__FILE__))

ip = MultiTkIp.new_safeTk{
  # When a block is given to 'new_safeTk' method,
  # the block is evaluated on $SAFE==4.
  ent.each{|pat, val| Tk.tk_call('option', 'add', pat, val)}
}

print "ip.eval_proc{$SAFE} ==> ", ip.eval_proc{$SAFE}, "\n"

print "\ncall 'ip.wait_on_mainloop = false'\n"
print "If 'ip.wait_on_mainloop? == true', ",
  "when 'mainloop' is called on 'ip.eval_proc', ",
  "'ip.eval_proc' does't return while the root window exists.\n",
  "If you want to avoid that, set wait_on_mainloop to false. ",
  "Then the mainloop in the eval_proc returns soon ",
  "and the following steps are evaluated. \n",
  "If you hate the both of them, use 'ip.bg_eval_proc' or ",
  "wrap 'ip.eval_proc' by a thread.\n"

ip.wait_on_mainloop = false

ret = ip.eval_proc{
  # When a block is given to 'eval_proc' method,
  # the block is evaluated on the IP's current safe level.
  # So, the followings raises an exception.
  # An Exception object of the exception is returned as a
  # return value of this method.

  load file
}
print "\nip.eval_proc{}, which includes insecure operiation in the given block, returns an exception object: ", ret.inspect, "\n"

print "If a proc object is given, the proc is evaluated on the safe-level which is kept on the proc :: ip.eval_proc( proc{$SAFE} ) ==> ", ip.eval_proc(proc{$SAFE}), "\n"

safe0_cmd = Proc.new{
  print 'safe0_cmd safe-level == ', $SAFE, "\n"
  # This proc object keeps current safe-level ($SAFE==0).
  load file
}
ip.eval_proc{safe0_cmd.call}

# Tk.mainloop is ignored on the slave-IP
Tk.mainloop
