#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

TkOption.add('*textBackground', 'white')

slb = Tk::Iwidgets::Scrolledlistbox.new(:selectmode=>:single,
                                        :vscrollmode=>:static,
                                        :hscrollmode=>:dynamic,
                                        :labeltext=>'List',
                                        :selectioncommand=>proc{
                                          puts(slb.get_curselection)
                                        },
                                        :dblclickcommand=>proc{
                                          puts('Double Click')
                                          puts(slb.get_curselection)
                                        })
slb.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

slb.insert('end', *['Hello', 'Out There', 'World'])

Tk.mainloop
