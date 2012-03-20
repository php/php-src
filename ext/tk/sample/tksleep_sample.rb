require 'tk'

v = TkVariable.new(0)
l = TkLabel.new(:textvariable=>v).pack(:pady=>[1, 10])

a = TkButton.new(:text=>"button A :: proc{p ['AAA', v.value]}").pack(:fill=>:x, :pady=>[1, 15], :padx=>15)
a.command{p ['AAA', v.value]}

TkLabel.new(:text=>'Callback of the button B returns LIFO order').pack
b = TkButton.new(:text=>"button B :: proc{n = v.value; p ['B:start', n]; Tk.sleep(10000); p ['B:end', n]}").pack(:fill=>:x, :pady=>[1, 15], :padx=>15)
b.command{n = v.value; p ['B:start', n]; Tk.sleep(10000); p ['B:end', n]}

TkLabel.new(:text=>'Callback of the button C returns FIFO order').pack
c = TkButton.new(:text=>"button C :: proc{n = v.value; Thread.new{p ['C:start', n]; Tk.sleep(10000); p ['C:end', n]}}").pack(:fill=>:x, :pady=>[1, 15], :padx=>15)
c.command{n = v.value; Thread.new{p ['C:start', n]; Tk.sleep(10000); p ['C:end', n]}}

TkLabel.new(:text=>'Callback of the button D blocks eventloop (no respond to event)').pack
d = TkButton.new(:text=>"button D :: proc{n = v.value; p ['D:start', n]; sleep(10); p ['D:end', n]}").pack(:fill=>:x, :pady=>[1,15], :padx=>15)
d.command{n = v.value; p ['D:start', n]; sleep(10); p ['D:end', n]}

TkLabel.new(:text=>'Callback of the button E is another way to avoid eventloop blocking').pack
e = TkButton.new(:text=>"button E :: proc{n = v.value; Thread.new{p ['D:start', n]; sleep(10); p ['D:end', n]}}").pack(:fill=>:x, :pady=>[1,15], :padx=>15)
e.command{n = v.value; Thread.new{p ['D:start', n]; sleep(10); p ['D:end', n]}}

TkButton.new(:text=>'QUIT', :command=>proc{exit}).pack

TkTimer.new(500, -1){v.numeric += 1}.start

Tk.mainloop
