#!/usr/bin/ruby

require 'tk'
require 'tkextlib/tcllib/plotchart'

TkCanvas.new(:background=>'white', :width=>400, :height=>200){|c|
  pack(:fill=>:both)
  Tk::Tcllib::Plotchart::XYPlot.new(c, [0.0, 100.0, 10.0],
                                       [0.0, 100.0, 20.0]){
    [ [0.0, 32.0], [10.0, 50.0], [25.0, 60.0], [78.0, 11.0] ].each{|x, y|
      plot('series1', x, y)
    }
    title("Data series")
  }
}

Tk.mainloop
