#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

pw = Tk::Iwidgets::Panedwindow.new(:width=>300, :height=>300)

top = pw.add
middle = pw.add(:margin=>10)
bottom = pw.add(:margin=>10, :minimum=>10)

pw.pack(:fill=>:both, :expand=>true)

pw.child_site_list.each{|pane|
  TkButton.new(pane, :text=>pane.path, :relief=>:raised,
               :borderwidth=>2).pack(:fill=>:both, :expand=>true)
}

pw.fraction(50,30,20)
pw.paneconfigure(0, :minimum=>20)  # 0 == pw.index(top)
pw.paneconfigure(bottom, :margin=>15)

Tk.mainloop
