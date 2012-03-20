#
#  tkextlib/iwidgets/scrolledframe.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Scrolledframe < Tk::Iwidgets::Scrolledwidget
    end
  end
end

class Tk::Iwidgets::Scrolledframe
  TkCommandNames = ['::iwidgets::scrolledframe'.freeze].freeze
  WidgetClassName = 'Scrolledframe'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def child_site
    window(tk_call(@path, 'childsite'))
  end

  def justify(dir)
    tk_call(@path, 'justify', dir)
    self
  end

  def xview(*index)
    if index.size == 0
      list(tk_send_without_enc('xview'))
    else
      tk_send_without_enc('xview', *index)
      self
    end
  end
  def xview_moveto(*index)
    xview('moveto', *index)
  end
  def xview_scroll(*index)
    xview('scroll', *index)
  end

  def yview(*index)
    if index.size == 0
      list(tk_send_without_enc('yview'))
    else
      tk_send_without_enc('yview', *index)
      self
    end
  end
  def yview_moveto(*index)
    yview('moveto', *index)
  end
  def yview_scroll(*index)
    yview('scroll', *index)
  end
end
