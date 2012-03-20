#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

rb = Tk::Iwidgets::Radiobox.new(:labeltext=>'Fonts')
rb.add('times',     :text=>'Times')
rb.add('helvetica', :text=>'Helvetica')
rb.add('courier',   :text=>'Courier')
rb.add('symbol',    :text=>'Symbol')
rb.select('courier')
rb.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

Tk.mainloop
