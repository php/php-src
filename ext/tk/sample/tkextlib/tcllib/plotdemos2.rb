#!/usr/bin/ruby

require 'tk'
require 'tkextlib/tcllib/plotchart'

###############################

c1 = TkCanvas.new(:background=>'white', :width=>400, :height=>200)
c2 = TkCanvas.new(:background=>'white', :width=>400, :height=>200)
Tk.pack(c1,c2, :fill=>:both, :side=>:top)

###############################
# Set up a strip chart
###############################
slipchart = Tk::Tcllib::Plotchart::Stripchart.new(c1, [0.0, 100.0, 10.0],
                                                      [0.0, 100.0, 20.0])

TkTimer.new(500, -1, proc{|obj| # obj --> TkTimer object
              slipchart, xold, xd, yold, yd = obj.return_value
              xnew = xold + xd
              ynew = yold + (rand() - 0.5) * yd
              ynew2 = yold + (rand() - 0.5) * 2.0 * yd
              slipchart.plot('series1', xnew, ynew)
              slipchart.plot('series2', xnew, ynew2)
              obj.stop if xnew >= 200
              [slipchart, xnew, xd, ynew, yd] # return_value
            }).start(100, proc{
                       # init return_value
                       [slipchart, 0.0, 15.0, 50.0, 30.0]
                     })

slipchart.title "Aha!"

###############################
# Set up an isometric plot
###############################
s = Tk::Tcllib::Plotchart::IsometricPlot.new(c2, [0.0, 100.0], [0.0, 200.0],
                                             :noaxes)

s.set_zoom_pan

s.plot('rectangle',        [10.0, 10.0, 50.0, 50.0], 'green')
s.plot('filled-rectangle', [20.0, 20.0, 40.0, 40.0], 'red')
s.plot('filled-circle',    [70.0, 70.0, 40.0], 'yellow')
s.plot('circle',           [70.0, 70.0, 42.0])

###############################
# Check the symbols
###############################
h = TkToplevel.new(:title=>'h')
c = TkCanvas.new(h, :bg=>'white', :width=>400, :height=>200).pack(:fill=>:both)

s = Tk::Tcllib::Plotchart::XYPlot.new(c, [0.0, 100.0, 10.0],
                                         [0.0, 100.0, 20.0])
s.dataconfig('series1', :colour=>'red',   :type=>:symbol)
s.dataconfig('series2', :colour=>'green', :type=>:both)

s.yconfig(:format=>"%12.2e")

x = 5.0
%w(plus cross circle up down dot upfilled downfilled).each{|sym|
  s.dataconfig('series1', :symbol=>sym)
  s.dataconfig('series2', :symbol=>sym)
  s.plot('series1', x, 50.0)
  s.plot('series2', x, 20)
  x += 10
}

##############################

Tk.mainloop
