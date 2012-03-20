#!/usr/bin/ruby

require 'tk'
require 'tkextlib/tcllib/plotchart'

###############################

c1 = TkCanvas.new(:background=>'white', :width=>400, :height=>200)
c2 = TkCanvas.new(:background=>'white', :width=>400, :height=>200)
c3 = TkCanvas.new(:background=>'white', :width=>400, :height=>200)
Tk.pack(c1,c2,c3, :fill=>:both, :side=>:top)

h = TkToplevel.new(:title=>'h')
hc1 = TkCanvas.new(h, :background=>'white', :width=>400, :height=>200)
hc2 = TkCanvas.new(h, :background=>'white', :width=>400, :height=>200)
Tk.pack(hc1,hc2, :fill=>:both, :side=>:top)

v = TkToplevel.new(:title=>'v')
vc1 = TkCanvas.new(v, :background=>'white', :width=>400, :height=>200)
vc2 = TkCanvas.new(v, :background=>'white', :width=>400, :height=>200)
vc3 = TkCanvas.new(v, :background=>'white', :width=>400, :height=>200)
Tk.pack(vc1,vc2,vc3, :fill=>:both, :side=>:top)

###############################

s = Tk::Tcllib::Plotchart::XYPlot.new(c1, [0.0, 100.0, 10.0],
                                          [0.0, 100.0, 20.0])


xd =    5.0
yd =   20.0
xold =  0.0
yold = 50.0

s.dataconfig('series1', :color=>'red')

(0..19).each{|i|
  xnew = xold + xd
  ynew = yold + (rand() - 0.5) * yd
  ynew2 = yold + (rand() - 0.5) * 2.0 * yd
  s.plot('series1', xnew, ynew)
  s.plot('series2', xnew, ynew2)
  xold = xnew
  yold = ynew
}

s.xtext "X-coordinate"
s.ytext "Y-data"
s.title "Aha!"

c1.wait_visibility

s.save_plot "aha.ps"

###############################

s = Tk::Tcllib::Plotchart::Piechart.new(c2)

s.plot([ ["Long names", 10], ["Short names", 30],
         ["Average", 40],    ["Ultra-short names", 5] ])

#
# Note: title should be shifted up
#       - distinguish a separate title area
#
s.title "Okay - this works"

###############################

s = Tk::Tcllib::Plotchart::PolarPlot.new(c3, [3.0, 1.0])

0.step(359, 10){|angle|
  rad = 1.0+Math.cos(angle*Math::PI/180.0)
  s.plot('cardioid', rad, angle)
}

s.title "Cardioid"

###############################

s = Tk::Tcllib::Plotchart::Barchart.new(hc1, %w(A B C D E),
                                        [0.0, 10.0, 2.0], 2)

s.plot('series1', [1.0, 4.0, 6.0, 1.0, 7.0], 'red')
s.plot('series2', [0.0, 3.0, 7.0, 9.3, 2.0], 'green')
s.title "Arbitrary data"

###############################

s = Tk::Tcllib::Plotchart::Barchart.new(hc2, %w(A B C D E),
                                        [0.0, 20.0, 5.0], :stacked)

s.plot('series1', [1.0, 4.0, 6.0, 1.0, 7.0], 'red')
s.plot('series2', [0.0, 3.0, 7.0, 9.3, 2.0], 'green')
s.title "Stacked diagram"

###############################

s = Tk::Tcllib::Plotchart::HorizontalBarchart.new(vc1, [0.0, 10.0, 2.0],
                                                  %w(A B C D E), 2)

s.plot('series1', [1.0, 4.0, 6.0, 1.0, 7.0], 'red')
s.plot('series2', [0.0, 3.0, 7.0, 9.3, 2.0], 'green')
s.title "Arbitrary data"

###############################

s = Tk::Tcllib::Plotchart::HorizontalBarchart.new(vc2, [0.0, 20.0, 5.0],
                                                  %w(A B C D E), :stacked)

s.plot('series1', [1.0, 4.0, 6.0, 1.0, 7.0], 'red')
s.plot('series2', [0.0, 3.0, 7.0, 9.3, 2.0], 'green')
s.title "Stacked diagram"

###############################

s = Tk::Tcllib::Plotchart::Timechart.new(vc3, "1 january 2004",
                                              "31 december 2004", 4)

s.period("Spring", "1 march 2004", "1 june 2004", 'green')
s.period("Summer", "1 june 2004", "1 september 2004", 'yellow')
s.vertline("1 jan", "1 january 2004")
s.vertline("1 apr", "1 april 2004")
s.vertline("1 jul", "1 july 2004")
s.vertline("1 oct", "1 october 2004")
s.milestone("Longest day", "21 july 2004")
s.title "Seasons (northern hemisphere)"

###############################

z = TkToplevel.new(:title=>'3D')

zc1 = TkCanvas.new(z, :background=>'white', :width=>400, :height=>300)
zc2 = TkCanvas.new(z, :background=>'white', :width=>400, :height=>250)
Tk.pack(zc1,zc2)

s = Tk::Tcllib::Plotchart::Plot3D.new(zc1,
                                      [0, 10, 3], [-10, 10, 10], [0, 10, 2.5])

s.title "3D Plot"
s.plot_function{|x, y|
  # cowboyhat
  x1 = x.to_f/9.0
  y1 = y.to_f/9.0
  3.0 * (1.0-(x1*x1+y1*y1))*(1.0-(x1*x1+y1*y1))
}

s = Tk::Tcllib::Plotchart::Plot3D.new(zc2,
                                      [0, 10, 3], [-10, 10, 10], [0, 10, 2.5])
s.title "3D Plot - data "
s.colour("green", "black")
s.plot_data([ [1.0, 2.0, 1.0, 0.0],
              [1.1, 3.0, 1.1, -0.5],
              [3.0, 1.0, 4.0, 5.0] ])

###############################

Tk.mainloop
