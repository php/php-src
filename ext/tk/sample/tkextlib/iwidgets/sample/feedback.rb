#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Tk::Iwidgets::Feedback.new(:labeltext=>'Status', :steps=>20){|fb|
  pack(:padx=>10, :pady=>10, :fill=>:both, :expand=>true)
  TkTimer.new(500, 20, proc{fb.step}).start(2500)
}

Tk.mainloop
