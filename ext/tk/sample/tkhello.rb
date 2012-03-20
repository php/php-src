require "tk"

TkButton.new(nil,
             :text => 'hello',
             :command => proc{print "hello\n"}).pack(:fill=>'x')
TkButton.new(nil,
             :text => 'quit',
             :command => proc{exit}).pack(:fill=>'x')

Tk.mainloop
