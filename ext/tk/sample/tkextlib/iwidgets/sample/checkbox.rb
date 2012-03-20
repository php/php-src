#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

cb = Tk::Iwidgets::Checkbox.new
cb.add('bold',      :text=>'Bold')
cb.add('italic',    :text=>'Italic')
cb.add('underline', :text=>'Underline')
cb.select('underline')
cb.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

Tk.mainloop
