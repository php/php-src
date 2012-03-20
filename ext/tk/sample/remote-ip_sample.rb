#!/usr/bin/env ruby
require 'remote-tk'

puts <<EOM
This sample controls the other Tk interpreter (Ruby/Tk, Tcl/Tk, and so on)
which running on the other process. For this purpose, Ruby/Tk uses Tcl/Tk's
'send' command. Availability of the command depends on your GUI environment.
If this script doesn't work, please check your environment (see Tcl/Tk FAQ).
EOM
#'

unless (wish = TkWinfo.interps.find{|ip| ip =~ /^wish/})
  puts ''
  puts 'Please start "wish" (Tcl/Tk shell) before running this sample script.'
  exit 1
end

ip = RemoteTkIp.new(wish)
ip.eval_proc{TkButton.new(:command=>proc{puts 'This procesure is on the controller-ip (Ruby/Tk)'}, :text=>'print on Ruby/Tk (controller-ip)').pack(:fill=>:x)}
ip.eval_proc{TkButton.new(:command=>'puts {This procesure is on the remote-ip (wish)}', :text=>'print on wish (remote-ip)').pack(:fill=>:x)}

# If your remote-ip is Ruby/Tk, you can control the remote Ruby by
# 'ruby' or 'ruby_eval' or 'ruby_cmd' on the Tk interpreter.
if ip.is_rubytk?
  ip.eval_proc{TkButton.new(:command=>'ruby {p 111; p Array.new(3,"ruby")}', :text=>'ruby cmd on the remote-ip').pack(:fill=>:x)}
end

ip.eval_proc{TkButton.new(:command=>'exit', :text=>'QUIT').pack(:fill=>:x)}

TkButton.new(:command=>proc{exit}, :text=>'QUIT',
             :padx=>10, :pady=>7).pack(:padx=>10, :pady=>7)

Tk.mainloop
