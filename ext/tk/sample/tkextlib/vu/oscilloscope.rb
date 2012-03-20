#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu/charts'

#---USAGE
info = "...the heart of a Realtime Oscilloscope, where the PCIs
joystick port gives 4 digtal inputs and 4 analog 8 bit
values with an resolution of ~1 millisecond (!!!)
running Realtime Linux.
<p> creates DEMO.ps
<B1-Motion> see what happens"
puts info

#---GEOMETRY
geo_fr  = [10, 10, 210, 180]
geo_ch0 = [10, 10, 210,  90]
geo_ch1 = [10, 90, 210, 180]
geo_t1  = [15, 88]

#---GUI
c = TkCanvas.new(:width=>220, :height=>190).pack(:fill=>:both, :expand=>true)

#---background
TkcRectangle.new(c, geo_fr, :width=>4, :fill=>'aquamarine3',
                 :tags=>['osc', 'frbg'])

#---channel 0
ch0 = Tk::Vu::TkcStripchart.new(c, geo_ch0,
                                :fill=>'', :jumpscroll=>false,
                                :outline=>'', :scaleline=>'',
                                :stripline=>'cyan', :tags=>['osc', 'ch0'])

#---channel 1
ch1 = Tk::Vu::TkcStripchart.new(c, geo_ch1,
                                :fill=>'', :jumpscroll=>0,
                                :outline=>'', :scaleline=>'',
                                :stripline=>'red', :tags=>['osc', 'ch1'])

#---frame
TkcRectangle.new(c, geo_fr, :width=>4, :tags=>['osc', 'frfg'])

#---position
txt1 = TkcText.new(c, geo_t1, :text=>"B1-Motion: X:%X\tY:%Y",
                   :anchor=>:nw, :tags=>['osc', 'txt1'])

#---BINDINGS
c.bind('B1-Motion', proc{|x, y, xx, yy|
         ch0[:values] = x
         ch1[:values] = y
         txt1[:text] = "B1-Motion: X:#{xx}\tY:#{yy}"
       }, '%x %y %X %Y')

Tk.root.bind('v', proc{
               puts ch0[:values].join(' ')
               puts ch0[:values].size
             })

Tk.root.bind('p', proc{
               c.postscript(:file=>'DEMO.ps')
               puts "DEMO.ps printed"
             })

Tk.root.bind('q', proc{exit})

#####################

Tk.mainloop
