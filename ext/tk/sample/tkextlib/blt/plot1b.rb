#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

graph = Tk::BLT::Graph.new.pack
plot = graph.element_create
plot.configure(:linewidth=>0, :label=>'foo',
               :data=>[[1.0, 3.4], [1.1, 2.8], [1.2, 3.1], [1.4, 2.9]].flatten)

Tk.mainloop
