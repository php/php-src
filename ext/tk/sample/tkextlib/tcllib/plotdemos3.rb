#!/usr/bin/ruby

require 'tk'
require 'tkextlib/tcllib/plotchart'

###############################
# Set up a strip chart
###############################

Tk::Tcllib::Plotchart::Stripchart.new([0.0, 100.0, 10.0],
                                      [0.0, 100.0, 20.0],
                                      :background=>'white',
                                      :width=>400, :height=>200){|chart|
  title "Aha!"
  pack(:fill=>:both, :side=>:top)

  series1 = Tk::Tcllib::Plotchart::PlotSeries.new(chart)
  series2 = Tk::Tcllib::Plotchart::PlotSeries.new(chart)

  xd = 15.0
  yd = 30.0

  TkTimer.new(500, -1, proc{|obj| # obj --> TkTimer object
                xold, yold = obj.return_value
                xnew = xold + xd
                ynew = yold + (rand() - 0.5) * yd
                ynew2 = yold + (rand() - 0.5) * 2.0 * yd

                series1.plot(xnew, ynew)
                series2.plot(xnew, ynew2)

                obj.stop if xnew >= 200

                [xnew, ynew] # return_value
              }).start(100, proc{ [0.0, 50.0] }) # init return_value
}

###############################
# Set up an isometric plot
###############################
Tk::Tcllib::Plotchart::IsometricPlot.new([0.0, 100.0], [0.0, 200.0], :noaxes,
                                         :background=>'white',
                                         :width=>400, :height=>200){|chart|
  pack(:fill=>:both, :side=>:top)
  set_zoom_pan

  chart.plot('rectangle',        [10.0, 10.0, 50.0, 50.0], 'green')
  chart.plot('filled-rectangle', [20.0, 20.0, 40.0, 40.0], 'red')
  chart.plot('filled-circle',    [70.0, 70.0, 40.0], 'yellow')
  chart.plot('circle',           [70.0, 70.0, 42.0])
}

###############################
# Check the symbols
###############################
TkToplevel.new(:title=>'h'){|h|
  Tk::Tcllib::Plotchart::XYPlot.new(h, [0.0, 100.0, 10.0],
                                       [0.0, 100.0, 20.0],
                                    :bg=>'white',
                                    :width=>400, :height=>200){|chart|
    pack(:fill=>:both)

    yconfig(:format=>"%12.2e")

    series1 = Tk::Tcllib::Plotchart::PlotSeries.new(chart, :colour=>'red',
                                                    :type=>:symbol)
    series2 = Tk::Tcllib::Plotchart::PlotSeries.new(chart, :colour=>'green',
                                                    :type=>:both)

    x = 5.0
    %w(plus cross circle up down dot upfilled downfilled).each{|sym|
      series1.dataconfig(:symbol=>sym)
      series2.dataconfig(:symbol=>sym)
      series1.plot(x, 50.0)
      series2.plot(x, 20)
      x += 10
    }
  }
}

##############################

Tk.mainloop
