#
#  tkextlib/iwidgets/scrolledhtml.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Scrolledhtml < Tk::Iwidgets::Scrolledtext
    end
  end
end

class Tk::Iwidgets::Scrolledhtml
  TkCommandNames = ['::iwidgets::scrolledhtml'.freeze].freeze
  WidgetClassName = 'Scrolledhtml'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'update'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'fontname' << 'link' << 'alink' << 'textbackground'
  end
  private :__strval_optkeys

  def __font_optkeys
    super() << 'fixedfont'
  end
  private :__font_optkeys

  def import(href)
    tk_call(@path, 'import', href)
    self
  end

  def import_link(href)
    tk_call(@path, 'import', '-link', href)
    self
  end

  def pwd
    tk_call(@path, 'pwd')
  end

  def render(htmltext, workdir=None)
    tk_call(@path, 'render', htmltext, workdir)
    self
  end

  def title
    tk_call(@path, 'title')
  end
end
