#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

djl = Tk::Iwidgets::Disjointlistbox.new.pack(:fill=>:both, :expand=>true,
                                             :padx=>10,  :pady=>10)
djl.set_lhs(*[0,2,4,5])
djl.set_rhs(3,6)

djl.insert_lhs(1,7,8)
djl.insert_rhs(9)

p djl.get_lhs
p djl.get_rhs

Tk.mainloop
