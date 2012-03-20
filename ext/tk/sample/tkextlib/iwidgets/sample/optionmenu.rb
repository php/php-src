#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

om = Tk::Iwidgets::Optionmenu.new(:labelmargin=>5, :labelpos=>:w,
                                  :labeltext=>"Operating System :")

om.insert('end', 'Unix', 'VMS', 'Linux', 'OS/2', 'Windows NT', 'DOS')
om.sort_ascending
om.select('Linux')

om.pack(:padx=>10, :pady=>10)

Tk.mainloop
