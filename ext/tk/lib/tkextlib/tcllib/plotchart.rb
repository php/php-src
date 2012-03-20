#
#  tkextlib/tcllib/plotchart.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Simple plotting and charting package
#
# (The following is the original description of the library.)
#
# Plotchart is a Tcl-only package that focuses on the easy creation of
# xy-plots, barcharts and other common types of graphical presentations.
# The emphasis is on ease of use, rather than flexibility. The procedures
# that create a plot use the entire canvas window, making the layout of the
# plot completely automatic.
#
# This results in the creation of an xy-plot in, say, ten lines of code:
# --------------------------------------------------------------------
#    package require Plotchart
#
#    canvas .c -background white -width 400 -height 200
#    pack   .c -fill both
#
#    #
#    # Create the plot with its x- and y-axes
#    #
#    set s [::Plotchart::createXYPlot .c {0.0 100.0 10.0} {0.0 100.0 20.0}]
#
#    foreach {x y} {0.0 32.0 10.0 50.0 25.0 60.0 78.0 11.0 } {
#        $s plot series1 $x $y
#    }
#
#    $s title "Data series"
# --------------------------------------------------------------------
#
# A drawback of the package might be that it does not do any data management.
# So if the canvas that holds the plot is to be resized, the whole plot must
# be redrawn. The advantage, though, is that it offers a number of plot and
# chart types:
#
#    * XY-plots like the one shown above with any number of data series.
#    * Stripcharts, a kind of XY-plots where the horizontal axis is adjusted
#      automatically. The result is a kind of sliding window on the data
#      series.
#    * Polar plots, where the coordinates are polar instead of cartesian.
#    * Isometric plots, where the scale of the coordinates in the two
#      directions is always the same, i.e. a circle in world coordinates
#      appears as a circle on the screen.
#      You can zoom in and out, as well as pan with these plots (Note: this
#      works best if no axes are drawn, the zooming and panning routines do
#      not distinguish the axes), using the mouse buttons with the control
#      key and the arrow keys with the control key.
#    * Piecharts, with automatic scaling to indicate the proportions.
#    * Barcharts, with either vertical or horizontal bars, stacked bars or
#      bars side by side.
#    * Timecharts, where bars indicate a time period and milestones or other
#      important moments in time are represented by triangles.
#    * 3D plots (both for displaying surfaces and 3D bars)
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('Plotchart', '0.9')
# TkPackage.require('Plotchart', '1.1')
# TkPackage.require('Plotchart', '1.6.3')
TkPackage.require('Plotchart')

module Tk
  module Tcllib
    module Plotchart
      PACKAGE_NAME = 'Plotchart'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('Plotchart')
        rescue
          ''
        end
      end
    end
  end
end

module Tk::Tcllib::Plotchart
  extend TkCore
  ############################
  def self.view_port(w, *args) # args := pxmin, pymin, pxmax, pymax
    tk_call_without_enc('::Plotchart::viewPort', w.path, *(args.flatten))
  end

  def self.world_coordinates(w, *args) # args := xmin, ymin, xmax, ymax
    tk_call_without_enc('::Plotchart::worldCoordinates',
                        w.path, *(args.flatten))
  end

  def self.world_3D_coordinates(w, *args)
    # args := xmin, ymin, zmin, xmax, ymax, zmax
    tk_call_without_enc('::Plotchart::world3DCoordinates',
                        w.path, *(args.flatten))
  end

  def self.coords_to_pixel(w, x, y)
    list(tk_call_without_enc('::Plotchart::coordsToPixel', w.path, x, y))
  end

  def self.coords_3D_to_pixel(w, x, y, z)
    list(tk_call_without_enc('::Plotchart::coords3DToPixel', w.path, x, y, z))
  end

  def self.plotconfig(*args)
    case args.length
    when 0, 1, 2
      # 0: (no args) --> list of chat types
      # 1: charttype --> list of components
      # 2: charttype, component --> list of properties
      simplelist(tk_call('::Plotchart::plotconfig', *args))
    when 3
      # 3: charttype, component, property --> current value
      tk_call('::Plotchart::plotconfig', *args)
    else
      # 4: charttype, component, property, value : set new value
      # 5+: Error on Tcl/Tk
      tk_call('::Plotchart::plotconfig', *args)
      nil
    end
  end

  def self.plotpack(w, dir, *plots)
    tk_call_without_enc('::Plotchart::plotpack', w.path, dir, *plots)
    w
  end

  def self.polar_coordinates(w, radmax)
    tk_call_without_enc('::Plotchart::polarCoordinates', w.path, radmax)
  end

  def self.polar_to_pixel(w, rad, phi)
    list(tk_call_without_enc('::Plotchart::polarToPixel', w.path, rad, phi))
  end

  def self.pixel_to_coords(w, x, y)
    list(tk_call_without_enc('::Plotchart::coordsToPixel', w.path, x, y))
  end

  def self.determine_scale(*args) # (xmin, xmax, inverted=false)
    tk_call_without_enc('::Plotchart::determineScale', *args)
  end

  def self.set_zoom_pan(w)
    tk_call_without_enc('::Plotchart::setZoomPan', w.path)
  end

  ############################
  module ChartMethod
    include TkCore

    def title(str)
      tk_call_without_enc(@chart, 'title', _get_eval_enc_str(str))
      self
    end

    def save_plot(filename)
      tk_call_without_enc(@chart, 'saveplot', filename)
      self
    end

    def xtext(str)
      tk_call_without_enc(@chart, 'xtext', _get_eval_enc_str(str))
      self
    end

    def ytext(str)
      tk_call_without_enc(@chart, 'ytext', _get_eval_enc_str(str))
      self
    end

    def xconfig(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'xconfig', *hash_kv(key, true))
      else
        tk_call(@chart, 'xconfig', "-#{key}",value)
      end
      self
    end

    def yconfig(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'yconfig', *hash_kv(key, true))
      else
        tk_call(@chart, 'yconfig', "-#{key}", value)
      end
      self
    end

    def background(part, color_or_image, dir)
      tk_call_without_enc(@chart, 'background',
                          part, color_or_image, dir)
      self
    end

    def xticklines(color=None)
      tk_call(@chart, 'xticklines', color)
      self
    end

    def yticklines(color=None)
      tk_call(@chart, 'yticklines', color)
      self
    end

    def legendconfig(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'legendconfig', *hash_kv(key, true))
      else
        tk_call(@chart, 'legendconfig', "-#{key}", value)
      end
      self
    end

    def legend(series, text)
      tk_call_without_enc(@chart, 'legend',
                          _get_eval_enc_str(series), _get_eval_enc_str(text))
      self
    end

    def balloon(*args) # args => (x, y, text, dir) or ([x, y], text, dir)
      if args[0].kind_of?(Array)
        # args => ([x, y], text, dir)
        x, y = args.shift
      else
        # args => (x, y, text, dir)
        x = args.shift
        y = args.shift
      end

      text, dir = args

      tk_call_without_enc(@chart, 'balloon', x, y,
                          _get_eval_enc_str(text), dir)
      self
    end

    def balloonconfig(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'balloonconfig', *hash_kv(key, true))
      else
        tk_call(@chart, 'balloonconfig', "-#{key}", value)
      end
    end

    def plaintext(*args) # args => (x, y, text, dir) or ([x, y], text, dir)
      if args[0].kind_of?(Array)
        # args => ([x, y], text, dir)
        x, y = args.shift
      else
        # args => (x, y, text, dir)
        x = args.shift
        y = args.shift
      end

      text, dir = args

      tk_call_without_enc(@chart, 'plaintext', x, y,
                          _get_eval_enc_str(text), dir)
      self
    end

    ############################

    def view_port(*args) # args := pxmin, pymin, pxmax, pymax
      tk_call_without_enc('::Plotchart::viewPort', @path, *(args.flatten))
      self
    end

    def world_coordinates(*args) # args := xmin, ymin, xmax, ymax
      tk_call_without_enc('::Plotchart::worldCoordinates',
                          @path, *(args.flatten))
      self
    end

    def world_3D_coordinates(*args)
      # args := xmin, ymin, zmin, xmax, ymax, zmax
      tk_call_without_enc('::Plotchart::world3DCoordinates',
                          @path, *(args.flatten))
      self
    end

    def coords_to_pixel(x, y)
      list(tk_call_without_enc('::Plotchart::coordsToPixel', @path, x, y))
    end

    def coords_3D_to_pixel(x, y, z)
      list(tk_call_without_enc('::Plotchart::coords3DToPixel', @path, x, y, z))
    end

    def plotpack(dir, *plots)
      tk_call_without_enc('::Plotchart::plotpack', @path, dir, *plots)
      self
    end

    def polar_coordinates(radmax)
      tk_call_without_enc('::Plotchart::polarCoordinates', @path, radmax)
      self
    end

    def polar_to_pixel(rad, phi)
      list(tk_call_without_enc('::Plotchart::polarToPixel', @path, rad, phi))
    end

    def pixel_to_coords(x, y)
      list(tk_call_without_enc('::Plotchart::coordsToPixel', @path, x, y))
    end

    def determine_scale(xmax, ymax)
      tk_call_without_enc('::Plotchart::determineScale', @path, xmax, ymax)
      self
    end

    def set_zoom_pan()
      tk_call_without_enc('::Plotchart::setZoomPan', @path)
      self
    end
  end

  ############################
  class XYPlot < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createXYPlot'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] xaxis, yaxis [, keys])
                          # xaxis := Array of [minimum, maximum, stepsize]
                          # yaxis := Array of [minimum, maximum, stepsize]
      if args[0].kind_of?(Array)
        @xaxis = args.shift
        @yaxis = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @xaxis = args.shift
        @yaxis = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@xaxis), array2tk_list(@yaxis))
    end
    private :_create_chart

    def __destroy_hook__
      Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.mutex.synchronize{
        Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.delete(@path)
      }
    end

    def plot(series, x, y)
      tk_call_without_enc(@chart, 'plot', _get_eval_enc_str(series), x, y)
      self
    end

    def contourlines(xcrd, ycrd, vals, clss=None)
      xcrd = array2tk_list(xcrd) if xcrd.kind_of?(Array)
      ycrd = array2tk_list(ycrd) if ycrd.kind_of?(Array)
      vals = array2tk_list(vals) if vals.kind_of?(Array)
      clss = array2tk_list(clss) if clss.kind_of?(Array)

      tk_call(@chart, 'contourlines', xcrd, ycrd, vals, clss)
      self
    end

    def contourfill(xcrd, ycrd, vals, clss=None)
      xcrd = array2tk_list(xcrd) if xcrd.kind_of?(Array)
      ycrd = array2tk_list(ycrd) if ycrd.kind_of?(Array)
      vals = array2tk_list(vals) if vals.kind_of?(Array)
      clss = array2tk_list(clss) if clss.kind_of?(Array)

      tk_call(@chart, 'contourfill', xcrd, ycrd, vals, clss)
      self
    end

    def contourbox(xcrd, ycrd, vals, clss=None)
      xcrd = array2tk_list(xcrd) if xcrd.kind_of?(Array)
      ycrd = array2tk_list(ycrd) if ycrd.kind_of?(Array)
      vals = array2tk_list(vals) if vals.kind_of?(Array)
      clss = array2tk_list(clss) if clss.kind_of?(Array)

      tk_call(@chart, 'contourbox', xcrd, ycrd, vals, clss)
      self
    end

    def color_map(colors)
      colors = array2tk_list(colors) if colors.kind_of?(Array)

      tk_call_without_enc(@chart, 'colorMap', colors)
      self
    end

    def grid_cells(xcrd, ycrd)
      xcrd = array2tk_list(xcrd) if xcrd.kind_of?(Array)
      ycrd = array2tk_list(ycrd) if ycrd.kind_of?(Array)

      tk_call_without_enc(@chart, 'grid', xcrd, ycrd)
      self
    end

    def dataconfig(series, key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'dataconfig', series, *hash_kv(key, true))
      else
        tk_call(@chart, 'dataconfig', series, "-#{key}", value)
      end
    end

    def rescale(xscale, yscale) # xscale|yscale => [newmin, newmax, newstep]
      tk_call_without_enc(@chart, 'rescale', xscale, yscale)
      self
    end

    def trend(series, xcrd, ycrd)
      tk_call_without_enc(@chart, 'trend',
                          _get_eval_enc_str(series), xcrd, ycrd)
      self
    end

    def rchart(series, xcrd, ycrd)
      tk_call_without_enc(@chart, 'rchart',
                          _get_eval_enc_str(series), xcrd, ycrd)
      self
    end

    def interval(series, xcrd, ymin, ymax, ycenter=None)
      tk_call(@chart, 'interval', series, xcrd, ymin, ymax, ycenter)
      self
    end

    def box_and_whiskers(series, xcrd, ycrd)
      tk_call_without_enc(@chart, 'box-and-whiskers',
                          _get_eval_enc_str(series), xcrd, ycrd)
      self
    end
    alias box_whiskers box_and_whiskers

    def vectorconfig(series, key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'vectorconfig',
                            _get_eval_enc_str(series), *hash_kv(key, true))
      else
        tk_call(@chart, 'vectorconfig', series, "-#{key}", value)
      end
      self
    end

    def vector(series, xcrd, ycrd, ucmp, vcmp)
      tk_call_without_enc(@chart, 'vector', _get_eval_enc_str(series),
                          xcrd, ycrd, ucmp, vcmp)
      self
    end

    def dotconfig(series, key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'dotconfig',
                            _get_eval_enc_str(series), *hash_kv(key, true))
      else
        tk_call(@chart, 'dotconfig', series, "-#{key}", value)
      end
      self
    end

    def dot(series, xcrd, ycrd, value)
      tk_call_without_enc(@chart, 'dot', _get_eval_enc_str(series),
                          xcrd, ycrd, value)
      self
    end
  end

  ############################
  class Stripchart < XYPlot
    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createStripchart'.freeze
    ].freeze
  end

  ############################
  class TXPlot < XYPlot
    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createTXPlot'.freeze
    ].freeze
  end

  ############################
  class XLogYPlot < XYPlot
    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createXLogYPlot'.freeze
    ].freeze
  end

  ############################
  class Histogram < XYPlot
    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createHistgram'.freeze
    ].freeze
  end

  ############################
  class PolarPlot < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createPolarplot'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] radius_data [, keys])
                          # radius_data := Array of [maximum_radius, stepsize]
      if args[0].kind_of?(Array)
        @radius_data = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @radius_data = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@radius_data))
    end
    private :_create_chart

    def __destroy_hook__
      Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.mutex.synchronize{
        Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.delete(@path)
      }
    end

    def plot(series, radius, angle)
      tk_call_without_enc(@chart, 'plot', _get_eval_enc_str(series),
                          radius, angle)
      self
    end

    def dataconfig(series, key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'dataconfig', _get_eval_enc_str(series),
                            *hash_kv(key, true))
      else
        tk_call(@chart, 'dataconfig', series, "-#{key}", value)
      end
    end
  end
  Polarplot = PolarPlot

  ############################
  class IsometricPlot < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createIsometricPlot'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] xaxis, yaxis, [, step] [, keys])
                          # xaxis := Array of [minimum, maximum]
                          # yaxis := Array of [minimum, maximum]
                          # step := Float of stepsize | "noaxes" | :noaxes
      if args[0].kind_of?(Array)
        @xaxis = args.shift
        @yaxis = args.shift

        if args[0].kind_of?(Hash)
          @stepsize = :noaxes
        else
          @stepsize = args.shift
        end

        super(*args) # create canvas widget
      else
        parent = args.shift

        @xaxis = args.shift
        @yaxis = args.shift

        if args[0].kind_of?(Hash)
          @stepsize = :noaxes
        else
          @stepsize = args.shift
        end

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@xaxis), array2tk_list(@yaxis),
                          @stepsize)
    end
    private :_create_chart

    def plot(type, *args)
      self.__send__("plot_#{type.to_s.tr('-', '_')}", *args)
    end

    def plot_rectangle(*args) # args := x1, y1, x2, y2, color
      tk_call_without_enc(@chart, 'plot', 'rectangle', *(args.flatten))
      self
    end

    def plot_filled_rectangle(*args) # args := x1, y1, x2, y2, color
      tk_call_without_enc(@chart, 'plot', 'filled-rectangle', *(args.flatten))
      self
    end

    def plot_circle(*args) # args := xc, yc, radius, color
      tk_call_without_enc(@chart, 'plot', 'circle', *(args.flatten))
      self
    end

    def plot_filled_circle(*args) # args := xc, yc, radius, color
      tk_call_without_enc(@chart, 'plot', 'filled-circle', *(args.flatten))
      self
    end
  end
  Isometricplot = IsometricPlot

  ############################
  class Plot3D < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::create3DPlot'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] xaxis, yaxis, zaxis [, keys])
                          # xaxis := Array of [minimum, maximum, stepsize]
                          # yaxis := Array of [minimum, maximum, stepsize]
                          # zaxis := Array of [minimum, maximum, stepsize]
      if args[0].kind_of?(Array)
        @xaxis = args.shift
        @yaxis = args.shift
        @zaxis = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @xaxis = args.shift
        @yaxis = args.shift
        @zaxis = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@xaxis),
                          array2tk_list(@yaxis),
                          array2tk_list(@zaxis))
    end
    private :_create_chart

    def plot_function(cmd=Proc.new)
      Tk.ip_eval("proc #{@path}_#{@chart} {x y} {#{install_cmd(cmd)} $x $y}")
      tk_call_without_enc(@chart, 'plotfunc', "#{@path}_#{@chart}")
      self
    end

    def plot_funcont(conts, cmd=Proc.new)
      conts = array2tk_list(conts) if conts.kind_of?(Array)
      Tk.ip_eval("proc #{@path}_#{@chart} {x y} {#{install_cmd(cmd)} $x $y}")
      tk_call_without_enc(@chart, 'plotfuncont', "#{@path}_#{@chart}", conts)
      self
    end

    def grid_size(nxcells, nycells)
      tk_call_without_enc(@chart, 'gridsize', nxcells, nycells)
      self
    end

    def plot_line(dat, color)
      # dat has to be provided as a 2 level array.
      # 1st level contains rows, drawn in y-direction,
      # and each row is an array whose elements are drawn in x-direction,
      # for the columns.
      tk_call_without_enc(@chart, 'plotline', dat, color)
      self
    end

    def plot_data(dat)
      # dat has to be provided as a 2 level array.
      # 1st level contains rows, drawn in y-direction,
      # and each row is an array whose elements are drawn in x-direction,
      # for the columns.
      tk_call_without_enc(@chart, 'plotdata', dat)
      self
    end

    def zconfig(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'zconfig', *hash_kv(key, true))
      else
        tk_call(@chart, 'zconfig', "-#{key}", value)
      end
      self
    end

    def colour(fill, border)
      # configure the colours to use for polygon borders and inner area
      tk_call_without_enc(@chart, 'colour', fill, border)
      self
    end
    alias colours colour
    alias colors  colour
    alias color   colour
  end

  ############################
  class Barchart3D < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::create3DBarchart'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] yaxis, nobars [, keys])
                          # yaxis  := Array of [minimum, maximum, stepsize]
                          # nobars := number of bars
      if args[0].kind_of?(Array)
        @yaxis = args.shift
        @nobars = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @yaxis = args.shift
        @nobars = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@yaxis), @nobars)
    end
    private :_create_chart

    def plot(label, yvalue, color)
      tk_call_without_enc(@chart, 'plot', _get_eval_enc_str(label),
                          _get_eval_enc_str(yvalue), color)
      self
    end

    def config(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'config', *hash_kv(key, true))
      else
        tk_call(@chart, 'config', "-#{key}", value)
      end
      self
    end
  end

  ############################
  class RibbonChart3D < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::create3DRibbonChart'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] names, yaxis, zaxis [, keys])
                          # names := Array of the series
                          # yaxis := Array of [minimum, maximum, stepsize]
                          # zaxis := Array of [minimum, maximum, stepsize]
      if args[0].kind_of?(Array)
        @names = args.shift
        @yaxis = args.shift
        @zaxis = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @names = args.shift
        @yaxis = args.shift
        @zaxis = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@names),
                          array2tk_list(@yaxis),
                          array2tk_list(@zaxis))
    end
    private :_create_chart

    def line(*args) # xypairs, color
      color = args.pop # last argument is a color
      xypairs = TkComm.slice_ary(args.flatten, 2) # regenerate xypairs
      tk_call_without_enc(@chart, 'line', xypairs, color)
      self
    end

    def area(*args) # xypairs, color
      color = args.pop # last argument is a color
      xypairs = TkComm.slice_ary(args.flatten, 2) # regenerate xypairs
      tk_call_without_enc(@chart, 'area', xypairs, color)
      self
    end

    def zconfig(key, value=None)
      if key.kind_of?(Hash)
        tk_call_without_enc(@chart, 'zconfig', *hash_kv(key, true))
      else
        tk_call(@chart, 'zconfig',"-#{key}", value)
      end
      self
    end
  end


  ############################
  class Piechart < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createPiechart'.freeze
    ].freeze

    def initialize(*args) # args := ([parent] [, keys])
      if args[0].kind_of?(Tk::Canvas)
        parent = args.shift
        @path = parent.path
      else
        super(*args) # create canvas widget
      end
      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path)
    end
    private :_create_chart

    def plot(*dat)  # argument is a list of [label, value]
      tk_call(@chart, 'plot', dat.flatten)
      self
    end

    def colours(*list)
      tk_call_without_enc(@chart, 'colours', *list)
      self
    end
    alias colors  colours
  end


  ############################
  class Radialchart < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createRadialchart'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] names, scale, style [, keys])
                          # radius_data := Array of [maximum_radius, stepsize]
      if args[0].kind_of?(Array)
        @names = args.shift
        @scale = args.shift
        @style = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @names = args.shift
        @scale = args.shift
        @style = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@names), @scale, @style)
    end
    private :_create_chart

    def __destroy_hook__
      Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.mutex.synchronize{
        Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.delete(@path)
      }
    end

    def plot(data, color, thickness)
      tk_call_without_enc(@chart, 'plot', _get_eval_enc_str(data),
                          color, thickness)
      self
    end

    def colours(*list)
      tk_call_without_enc(@chart, 'colours', *list)
      self
    end
    alias colors  colours
  end

  ############################
  class Barchart < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createBarchart'.freeze
    ].freeze

    def initialize(*args)
      # args := ([parent,] xlabels, ylabels [, series] [, keys])
      # xlabels, ylabels := labels | axis ( depend on normal or horizontal )
      # labels := Array of [label, label, ...]
      #   (It determines the number of bars that will be plotted per series.)
      # axis := Array of [minimum, maximum, stepsize]
      # series := Integer number of data series | 'stacked' | :stacked
      if args[0].kind_of?(Array)
        @xlabels = args.shift
        @ylabels  = args.shift

        if args[0].kind_of?(Hash)
          @series_size = :stacked
        else
          @series_size  = args.shift
        end

        super(*args) # create canvas widget
      else
        parent = args.shift

        @xlabels = args.shift
        @ylabels = args.shift

        if args[0].kind_of?(Hash)
          @series_size = :stacked
        else
          @series_size  = args.shift
        end

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@xlabels), array2tk_list(@ylabels),
                          @series_size)
    end
    private :_create_chart

    def __destroy_hook__
      Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.mutex.synchronize{
        Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.delete(@path)
      }
    end

    def plot(series, dat, col=None)
      tk_call(@chart, 'plot', series, dat, col)
      self
    end

    def colours(*cols)
      # set the colours to be used
      tk_call(@chart, 'colours', *cols)
      self
    end
    alias colour colours
    alias colors colours
    alias color  colours
  end

  ############################
  class HorizontalBarchart < Barchart
    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createHorizontalBarchart'.freeze
    ].freeze
  end

  ############################
  class Boxplot < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createBoxplot'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] xaxis, ylabels [, keys])
                          # xaxis := Array of [minimum, maximum, stepsize]
                          # yaxis := List of labels for the y-axis
      if args[0].kind_of?(Array)
        @xaxis   = args.shift
        @ylabels = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @xaxis   = args.shift
        @ylabels = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@xaxis), array2tk_list(@ylabels))
    end
    private :_create_chart

    def __destroy_hook__
      Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.mutex.synchronize{
        Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.delete(@path)
      }
    end

    def plot(label, *values)
      tk_call(@chart, 'plot', label, values.flatten)
      self
    end
  end

  ############################
  class RightAxis < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createRightAxis'.freeze
    ].freeze

    def initialize(*args) # args := ([parent,] yaxis [, keys])
                          # yaxis := Array of [minimum, maximum, stepsize]
      if args[0].kind_of?(Array)
        @yaxis   = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @yaxis   = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          array2tk_list(@yaxis))
    end
    private :_create_chart

    def __destroy_hook__
      Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.mutex.synchronize{
        Tk::Tcllib::Plotchart::PlotSeries::SeriesID_TBL.delete(@path)
      }
    end
  end

  ############################
  class Timechart < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createTimechart'.freeze
    ].freeze

    def initialize(*args)
      # args := ([parent,] time_begin, time_end, items [, keys])
      # time_begin := String of time format (e.g. "1 january 2004")
      # time_end   := String of time format (e.g. "1 january 2004")
      # items := Expected/maximum number of items
      #          ( This determines the vertical spacing. )
      if args[0].kind_of?(String)
        @time_begin = args.shift
        @time_end   = args.shift
        @items      = args.shift

        super(*args) # create canvas widget
      else
        parent = args.shift

        @time_begin = args.shift
        @time_end   = args.shift
        @items      = args.shift

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call_without_enc(self.class::TkCommandNames[1], @path,
                          @time_begin, @time_end, @items)
    end
    private :_create_chart

    def period(txt, time_begin, time_end, col=None)
      tk_call(@chart, 'period', txt, time_begin, time_end, col)
      self
    end

    def milestone(txt, time, col=None)
      tk_call(@chart, 'milestone', txt, time, col)
      self
    end

    def vertline(txt, time)
      tk_call(@chart, 'vertline', txt, time)
      self
    end

    def hscroll=(scr)
      tk_call_without_enc(@chart, 'hscroll', scr)
      scr
    end
    def hscroll(scr)
      tk_call_without_enc(@chart, 'hscroll', scr)
      self
    end

    def vscroll=(scr)
      tk_call_without_enc(@chart, 'vscroll', scr)
      scr
    end
    def vscroll(scr)
      tk_call_without_enc(@chart, 'vscroll', scr)
      self
    end
  end

  ############################
  class Ganttchart < Tk::Canvas
    include ChartMethod

    TkCommandNames = [
      'canvas'.freeze,
      '::Plotchart::createGanttchart'.freeze
    ].freeze

    def initialize(*args)
      # args := ([parent,] time_begin, time_end, items [, text_width] [, keys])
      # time_begin := String of time format (e.g. "1 january 2004")
      # time_end   := String of time format (e.g. "1 january 2004")
      # args := Expected/maximum number of items
      #          ( This determines the vertical spacing. ),
      #         Expected/maximum width of items,
      #         Option Hash ( { key=>value, ... } )
      if args[0].kind_of?(String)
        @time_begin = args.shift
        @time_end   = args.shift
        @args  = args

        super(*args) # create canvas widget
      else
        parent = args.shift

        @time_begin = args.shift
        @time_end   = args.shift
        @args  = args

        if parent.kind_of?(Tk::Canvas)
          @path = parent.path
        else
          super(parent, *args) # create canvas widget
        end
      end

      @chart = _create_chart
    end

    def _create_chart
      p self.class::TkCommandNames[1] if $DEBUG
      tk_call(self.class::TkCommandNames[1], @path,
              @time_begin, @time_end, *args)
    end
    private :_create_chart

    def task(txt, time_begin, time_end, completed=0.0)
      list(tk_call(@chart, 'task', txt, time_begin, time_end,
                   completed)).collect!{|id|
        TkcItem.id2obj(self, id)
      }
    end

    def milestone(txt, time, col=None)
      tk_call(@chart, 'milestone', txt, time, col)
      self
    end

    def vertline(txt, time)
      tk_call(@chart, 'vertline', txt, time)
      self
    end

    def connect(from_task, to_task)
      from_task = array2tk_list(from_task) if from_task.kind_of?(Array)
      to_task   = array2tk_list(to_task)   if to_task.kind_of?(Array)

      tk_call(@chart, 'connect', from_task, to_task)
      self
    end

    def summary(txt, tasks)
      tasks = array2tk_list(tasks) if tasks.kind_of?(Array)
      tk_call(@chart, 'summary', tasks)
      self
    end

    def color_of_part(keyword, newcolor)
      tk_call(@chart, 'color', keyword, newcolor)
      self
    end

    def font_of_part(keyword, newfont)
      tk_call(@chart, 'font', keyword, newfont)
      self
    end

    def hscroll=(scr)
      tk_call_without_enc(@chart, 'hscroll', scr)
      scr
    end
    def hscroll(scr)
      tk_call_without_enc(@chart, 'hscroll', scr)
      self
    end

    def vscroll=(scr)
      tk_call_without_enc(@chart, 'vscroll', scr)
      scr
    end
    def vscroll(scr)
      tk_call_without_enc(@chart, 'vscroll', scr)
      self
    end
  end

  ############################
  class PlotSeries < TkObject
    SeriesID_TBL = TkCore::INTERP.create_table

    (Series_ID = ['series'.freeze, TkUtil.untrust('00000')]).instance_eval{
      @mutex = Mutex.new
      def mutex; @mutex; end
      freeze
    }
    TkCore::INTERP.init_ip_env{
      SeriesID_TBL.mutex.synchronize{ SeriesID_TBL.clear }
    }

    def self.id2obj(chart, id)
      path = chart.path
      SeriesID_TBL.mutex.synchronize{
        if SeriesID_TBL[path]
          SeriesID_TBL[path][id]? SeriesID_TBL[path][id]: id
        else
          id
        end
      }
    end

    def initialize(chart, keys=nil)
      @parent = @chart_obj = chart
      @ppath = @chart_obj.path
      Series_ID.mutex.synchronize{
        @path = @series = @id = Series_ID.join(TkCore::INTERP._ip_id_)
        Series_ID[1].succ!
      }
      SeriesID_TBL.mutex.synchronize{
        SeriesID_TBL[@ppath] ||= {}
        SeriesID_TBL[@ppath][@id] = self
      }
      dataconfig(keys) if keys.kind_of?(Hash)
    end

    def plot(*args)
      @chart_obj.plot(@series, *args)
    end

    def dataconfig(key, value=None)
      @chart_obj.dataconfig(@series, key, value)
    end
  end
end
