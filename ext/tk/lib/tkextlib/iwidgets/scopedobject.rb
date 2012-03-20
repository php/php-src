#
#  tkextlib/iwidgets/buttonbox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Scopedobject < TkObject
    end
  end
end

class Tk::Iwidgets::Scopedobject
  TkCommandNames = ['::iwidgets::scopedobject'.freeze].freeze
  WidgetClassName = 'Scopedobject'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def initialize(obj_name, keys={})
    @path = tk_call(self.class::TkCommandNames[0], obj_name, *hash_kv(keys))
  end
end
