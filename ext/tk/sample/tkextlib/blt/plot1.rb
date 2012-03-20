#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

graph = Tk::BLT::Graph.new.pack
plot = Tk::BLT::PlotComponent::Element.new(graph, :linewidth=>0, :label=>'foo')
plot.data([[1.0, 3.4], [1.1, 2.8], [1.2, 3.1], [1.4, 2.9]].flatten)

Tk.mainloop
