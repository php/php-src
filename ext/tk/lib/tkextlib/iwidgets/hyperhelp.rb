#
#  tkextlib/iwidgets/hyperhelp.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Hyperhelp < Tk::Iwidgets::Shell
    end
  end
end

class Tk::Iwidgets::Hyperhelp
  TkCommandNames = ['::iwidgets::hyperhelp'.freeze].freeze
  WidgetClassName = 'Hyperhelp'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'helpdir'
  end
  private :__strval_optkeys

  def __listval_optkeys
    super() << 'topics'
  end
  private :__listval_optkeys

  def show_topic(topic)
    tk_call(@path, 'showtopic', topic)
    self
  end

  def follow_link(href)
    tk_call(@path, 'followlink', href)
    self
  end

  def forward
    tk_call(@path, 'forward')
    self
  end

  def back
    tk_call(@path, 'back')
    self
  end
end
