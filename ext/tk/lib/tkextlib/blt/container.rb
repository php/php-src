#
#  tkextlib/blt/container.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Container < TkWindow
    TkCommandNames = ['::blt::container'.freeze].freeze
    WidgetClassName = 'Container'.freeze
    WidgetClassNames[WidgetClassName] ||= self

    def __strval_optkeys
      super() << 'name'
    end
    private :__strval_optkeys

    def find_command(pat)
      Hash[*simplelist(tk_send_without_enc('find', '-command', pat))]
    end

    def find_name(pat)
      Hash[*simplelist(tk_send_without_enc('find', '-name', pat))]
    end
  end
end
