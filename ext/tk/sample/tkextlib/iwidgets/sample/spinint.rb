#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

TkOption.add('*textBackground', 'white')

Tk::Iwidgets::Spinint.new(:labeltext=>'Temperature', :labelpos=>:w, :width=>5,
                          :fixed=>true, :range=>[32, 212]).pack(:pady=>10)

Tk.mainloop
