#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/blt'

load File.join(File.dirname(File.expand_path(__FILE__)),
               'scripts', 'stipples.rb')

TkOption.add('*graph.x.Title', 'X Axis Label')
TkOption.add('*graph.y.Title', 'Y Axis Label')
TkOption.add('*graph.title', 'A Simple Barchart')
TkOption.add('*graph.x.Font', 'Times 10')
TkOption.add('*graph.Element.Relief', :raised)

visual = Tk.root.winfo_screenvisual
if visual != 'staticgray' && visual != 'grayscale'
  TkOption.add('*graph.LineMarker.color', 'yellow')
  TkOption.add('*graph.Element.Background', 'white')
  TkOption.add('*graph.Legend.activeForeground', 'pink')
  TkOption.add('*print.background', 'yellow')
  TkOption.add('*quit.background',  'red')
  TkOption.add('*graph.background', 'palegreen')
  TkOption.add('*graph.plotBackground', 'lightblue')
end

htext = Tk::BLT::Htext.new(:widgetname=>'.htext', :text=><<EOD)
    This is an example of the barchart widget.  The barchart has
    many components; x and y axis, legend, crosshairs, elements, etc.
    To create a postscript file "bar.ps", press the %%

    ruby {
    b = TkButton.new(Tk::BLT::Htext::Htext_Widget.window,
                     :widgetname=>'print', :text=>'Print',
                     :command=>proc{
                        $graph.postsript(:output=>'bar.ps')
                     })
    Tk::BLT::Htext::Htext_Widget.window.append(b)
    }

%% button.
%%

    ruby {
    $graph = Tk::BLT::Barchart.new(:widgetname=>'.htext.graph',
                                   :relief=>:raised, :borderwidth=>2)
    $graph.xaxis_configure(:rotate=>90, :stepsize=>0)
    Tk::BLT::Htext::Htext_Widget.window.append($graph,
                                               :fill=>:both, :padx=>4)
    }

%%
    Hit the %%

    ruby {
    b = TkButton.new(Tk::BLT::Htext::Htext_Widget.window,
                     :widgetname=>'quit', :text=>'Quit',
                     :command=>proc{ exit })
    Tk::BLT::Htext::Htext_Widget.window.append(b)
    }

%% button when you've seen enough.%%

    ruby {
    l = TkLabel.new(Tk::BLT::Htext::Htext_Widget.window, :bitmap=>'BLT')
    Tk::BLT::Htext::Htext_Widget.window.append(l, :padx=>20)
    }

%%
EOD

names = %w(One Two Three Four Five Six Seven Eight)
if visual == 'staticgray' || visual == 'grayscale'
  fgcolors = %w(white white white white white white white white)
  bgcolors = %w(black black black black black black black black)
else
  fgcolors = %w(yellow orange red magenta purple blue cyan green)
  bgcolors = %w(yellow4 orange4 red4 magenta4 purple4 blue4 cyan4 green4)
end

numColors = names.length

Tk::TCL_PRECISION.value = 15

x = Tk::BLT::Vector.new
y = Tk::BLT::Vector.new
x.seq(-5.0, 5.0, 0.2)
y.expr("sin(#{x})")
barWidth = 0.19

$graph.element_create('sin', :relief=>:raised, :borderwidth=>1,
                      :x=>x, :y=>y, :barwidth=>barWidth)

Tk::BLT::Table.add(Tk.root, htext, :fill=>:both)

Tk.root.minsize(0, 0)

Tk::BLT.zoom_stack($graph)
Tk::BLT.crosshairs($graph)
Tk::BLT.active_legend($graph)
Tk::BLT.closest_point($graph)

Tk.mainloop
