#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

file = File.join(File.dirname(File.expand_path(__FILE__)),
                 'images', 'buckskin.gif')
bgTexture = TkPhotoImage.new(:file=>file)

TkOption.add('*Graph.Tile', bgTexture)
TkOption.add('*Label.Tile', bgTexture)
TkOption.add('*Frame.Tile', bgTexture)
TkOption.add('*Htext.Tile', bgTexture)
TkOption.add('*TileOffset', 0)
TkOption.add('*HighlightThickness',   0)
TkOption.add('*Element.ScaleSybols',  false)
TkOption.add('*Element.Smooth',       :linear)
TkOption.add('*activeLine.Color',     'yellow4')
TkOption.add('*activeLine.Fill',      'yellow')
TkOption.add('*activeLine.LineWidth', 0)
TkOption.add('*Element.Pixels',       3)
TkOption.add('*Graph.halo',           '7i')

if Tk.root.winfo_screenvisual != 'staticgray'
  TkOption.add('*print.background', 'yellow')
  TkOption.add('*quit.background',  'red')
end

length = 250000
graph = Tk::BLT::Graph.new(:title=>"Scatter Plot\n#{length} points")
graph.xaxis_configure(:loose=>false, :title=>'X Axis Label')
graph.yaxis_configure(:title=>'Y Axis Label')
graph.legend_configure(:activerelief=>:sunken, :background=>'')

Tk::BLT::Table.add(Tk.root, graph, [0,0], :fill=>:both)

v_x = Tk::BLT::Vector.new(length)
v_y = Tk::BLT::Vector.new(length)
v_x.expr("random(#{v_x})")
v_y.expr("random(#{v_y})")
v_x.sort(v_y)

plot = Tk::BLT::PlotComponent::Element.new(graph, :symbol=>:square,
                                           :color=>'green4', :fill=>'green2',
                                           :linewidth=>0, :outlinewidth=>1,
                                           :pixels=>4, :label=>'plot',
                                           :xdata=>v_x, :ydata=>v_y)

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
