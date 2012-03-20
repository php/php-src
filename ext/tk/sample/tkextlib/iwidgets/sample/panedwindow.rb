#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

pw = Tk::Iwidgets::Panedwindow.new(:width=>300, :height=>300)

pw.add('top')
pw.add('middle', :margin=>10)
pw.add('bottom', :margin=>10, :minimum=>10)

pw.pack(:fill=>:both, :expand=>true)

pw.child_site_list.each{|pane|
  TkButton.new(pane, :text=>pane.path, :relief=>:raised,
               :borderwidth=>2).pack(:fill=>:both, :expand=>true)
}

pw.fraction(50,30,20)
pw.paneconfigure(0, :minimum=>20)
pw.paneconfigure('bottom', :margin=>15)

Tk.mainloop
