#!/usr/bin/env ruby
require 'tcltklib'

master = TclTkIp.new
slave_name = 'slave0'
slave = master.create_slave(slave_name, true)
master._eval("::safe::interpInit #{slave_name}")
master._eval("::safe::loadTk #{slave_name}")

master._invoke('label', '.l1', '-text', 'master')
master._invoke('pack', '.l1', '-padx', '30', '-pady', '50')
master._eval('label .l2 -text {root widget of master-ip}')
master._eval('pack .l2 -padx 30 -pady 50')

slave._invoke('label', '.l1', '-text', 'slave')
slave._invoke('pack', '.l1', '-padx', '30', '-pady', '50')
slave._eval('label .l2 -text {root widget of slave-ip}')
slave._eval('pack .l2 -padx 30 -pady 20')
slave._eval('label .l3 -text {( container frame widget of master-ip )}')
slave._eval('pack .l3 -padx 30 -pady 20')

TclTkLib.mainloop
