#
#  tkextlib/tcllib/diagrams.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Draw diagrams
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('Diagrams', '0.3')
TkPackage.require('Diagrams')

module Tk::Tcllib
  module Diagrams
    PACKAGE_NAME = 'Diagrams'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('Diagrams')
      rescue
        ''
      end
    end
  end
end

class << Tk::Tcllib::Diagrams
  include TkCore

  def drawin(canvas)
    tk_call('::Diagrams::drawin', canvas)
    canvas
  end
  alias draw_in drawin

  def saveps(filename)
    tk_call('::Diagrams::saveps', filename)
    filename
  end
  alias save_ps saveps

  def direction(dir)
    tk_call('::Diagrams::direction', dir)
    dir
  end

  def currentpos(pos)
    list(tk_call('::Diagrams::currentpos', pos))
  end
  alias current_pos currentpos
  alias currentpos= currentpos
  alias current_pos= currentpos

  def getpos(anchor, obj)
    list(tk_call('::Diagrams::getpos', anchor, obj))
  end
  alias get_pos getpos

  def position(x, y)
    list(tk_call('::Diagrams::position', x, y))
  end

  def box(text, width=nil, height=nil)
    if width || height
      width  = '' unless width
      height = '' unless height
      list(tk_call('::Diagrams::box', text, width, height))
    else
      list(tk_call('::Diagrams::box', text))
    end
  end

  def plaintext(text, width=nil, height=nil)
    if width || height
      width  = '' unless width
      height = '' unless height
      list(tk_call('::Diagrams::plaintext', text, width, height))
    else
      list(tk_call('::Diagrams::plaintext', text))
    end
  end

  def circle(text, radius=nil)
    if radius
      list(tk_call('::Diagrams::circle', text, radius))
    else
      list(tk_call('::Diagrams::circle', text))
    end
  end

  def slanted(text, width=nil, height=nil, angle=nil)
    if width || height || angle
      width  = '' unless width
      height = '' unless height
      if angle
        list(tk_call('::Diagrams::slanted', text, width, height, angle))
      else
        list(tk_call('::Diagrams::slanted', text, width, height))
      end
    else
      list(tk_call('::Diagrams::slanted', text))
    end
  end

  def diamond(text, width=nil, height=nil)
    if width || height
      width  = '' unless width
      height = '' unless height
      list(tk_call('::Diagrams::diamond', text, width, height))
    else
      list(tk_call('::Diagrams::diamond', text))
    end
  end

  def drum(text, width=nil, height=nil)
    if width || height
      width  = '' unless width
      height = '' unless height
      list(tk_call('::Diagrams::drum', text, width, height))
    else
      list(tk_call('::Diagrams::drum', text))
    end
  end

  def arrow(text=nil, length=nil, head=nil)
    if length || head
      text   = '' unless text
      length = '' unless length
      list(tk_call('::Diagrams::arrow', text, length, head))
    else
      if text
        list(tk_call('::Diagrams::arrow', text))
      else
        list(tk_call('::Diagrams::arrow'))
      end
    end
  end

  def line(*args)
    ary = []
    args.each{|arg|
      if arg.kind_of?(Array) && arg.length == 2  # [length, angle]
        ary.concat arg
      else # ["POSITION", x, y] or length or angle
        ary << arg
      end
    }
    list(tk_call('::Diagrams::line', *ary))
  end

  def bracket(dir, dist, from_pos, to_pos)
    list(tk_call('::Diagrams::bracket', dir, dist, from_pos, to_pos))
  end

  def attach(anchor=None)
    tk_call('::Diagrams::attach', anchor)
  end

  def color(name=None)
    tk_call('::Diagrams::color', name)
  end

  def fillcolor(name=None)
    tk_call('::Diagrams::fillcolor', name)
  end

  def textcolor(name=None)
    tk_call('::Diagrams::textcolor', name)
  end

  def usegap(mode=None)
    bool(tk_call('::Diagrams::usegap', mode))
  end
  alias use_gap usegap

  def xgap(val=None)
    number(tk_call('::Diagrams::xgap', val))
  end

  def ygap(val=None)
    number(tk_call('::Diagrams::ygap', val))
  end

  def textfont(fnt=None)
    tk_call('::Diagrams::textfont', fnt)
  end

  def linewidth(pixels=None)
    number(tk_call('::Diagrams::linewidth', pixels))
  end

  def linestyle(style=None)
    tk_call('::Diagrams::linestyle', style)
  end

  def pushstate
    tk_call('::Diagrams::pushstate')
  end
  alias push_state pushstate

  def popstate
    tk_call('::Diagrams::popstate')
  end
  alias pop_state popstate

  def computepos
    list(tk_call('::Diagrams::computepos'))
  end
  alias compute_pos computepos

  def boxcoords(x1, y1, x2, y2)
    list(tk_call('::Diagrams::boxcoords', x1, y1, x2, y2))
  end

  def moveobject(obj)
    list(tk_call('::Diagrams::moveobject', obj))
  end
  alias move_object moveobject
end
