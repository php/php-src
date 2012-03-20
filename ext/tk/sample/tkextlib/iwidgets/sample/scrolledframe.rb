#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

sf = Tk::Iwidgets::Scrolledframe.new(:width=>150, :height=>180,
                                     :labeltext=>'scrolledframe')
cs = sf.child_site

TkButton.new(cs, :text=>'Hello').pack(:pady=>10)
TkButton.new(cs, :text=>'World').pack(:pady=>10)
TkButton.new(cs, :text=>'This is a test').pack(:pady=>10)
TkButton.new(cs, :text=>'This is a really big button').pack(:pady=>10)
TkButton.new(cs, :text=>'This is another really big button').pack(:pady=>10)
TkButton.new(cs, :text=>'This is the last really big button').pack(:pady=>10)

sf.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

Tk.mainloop
