#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

TkOption.add('*textBackground', 'white')

sh = Tk::Iwidgets::Scrolledhtml.new(:fontname=>'helvetica',
                                    :linkcommand=>proc{|href|
                                      sh.import_link(href)
                                    })
sh.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

sh.import(Tk.getOpenFile(:title=>'select HTML document'))

Tk.mainloop
