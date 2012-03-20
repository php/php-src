#!/usr/bin/env ruby

require 'remote-tk'

# start sub-process
ip_name = 'remote_ip'
ip_list = TkWinfo.interps
fork{
  exec "/usr/bin/env ruby -r tk -e \"Tk.appname('#{ip_name}');Tk.mainloop\""
}
sleep 1 until (app = (TkWinfo.interps - ip_list)[0]) && app =~ /^#{ip_name}/
p TkWinfo.interps

# create RemoteTkIp object
ip = RemoteTkIp.new(app)

# setup remote-ip window
btns = []
ip.eval_proc{
  btns <<
    TkButton.new(:command=>proc{
                   puts 'This procesure is on the controller-ip (Ruby-side)'
                 },
                 :text=>'print on controller-ip (Ruby-side)').pack(:fill=>:x)

  btns <<
    TkButton.new(:command=>
                   'puts {This procesure is on the remote-ip (Tk-side)}',
                 :text=>'print on remote-ip (Tk-side)').pack(:fill=>:x)

  btns <<
    TkButton.new(:command=>
                   'ruby {
                     puts "This procedure is on the remote-ip (Ruby-side)"
                     p Array.new(3,"ruby")
                    }',
                 :text=>'ruby cmd on the remote-ip').pack(:fill=>:x)

  TkButton.new(:command=>'exit', :text=>'QUIT').pack(:fill=>:x)
}

# setup controller-ip window
btns.each_with_index{|btn, idx|
  # The scope of the eval-block of 'eval_proc' method is different from
  # the enternal. If you want to pass local values to the eval-block,
  # use arguments of eval_proc method. They are passed to block-arguments.
  TkButton.new(:command=>proc{ip.eval_proc(btn){|b| b.flash}},
               :text=>"flash button-#{idx}",
               :padx=>10).pack(:padx=>10, :pady=>2)
}

TkButton.new(:command=>proc{exit}, :text=>'QUIT',
             :padx=>10, :pady=>7).pack(:padx=>10, :pady=>7)

# start eventloop
Tk.mainloop
