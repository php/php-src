#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

length = 250000
graph = Tk::BLT::Graph.new(:title=>"Scatter Plot\n#{length} points")
graph.xaxis_configure(:loose=>false, :title=>'X Axis Label')
graph.yaxis_configure(:title=>'Y Axis Label')
graph.legend_configure(:activerelief=>:sunken, :background=>'')

Tk::BLT::Table.add(Tk.root, graph, [0,0], :fill=>:both)

x = Array.new(length)
y = Array.new(length)
(0...length).each{|i|
  x[i] = rand
  y[i] = rand
}

plot = Tk::BLT::PlotComponent::Element.new(graph, :symbol=>:square,
                                           :color=>'green4', :fill=>'green2',
                                           :linewidth=>0, :outlinewidth=>1,
                                           :pixels=>4, :label=>'plot',
                                           :xdata=>x, :ydata=>y)

Tk.root.minsize(0, 0)

#graph.zoom_stack
#graph.crosshairs
#graph.active_legend
#graph.closest_point
Tk::BLT.zoom_stack(graph)
Tk::BLT.crosshairs(graph)
Tk::BLT.active_legend(graph)
Tk::BLT.closest_point(graph)

Tk::BLT::Busy.hold(graph)
Tk.update
Tk::BLT::Busy.release(graph)

Tk.mainloop
