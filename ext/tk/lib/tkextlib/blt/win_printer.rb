#
#  tkextlib/blt/win_printer.rb
#
#      *** Windows only ***
#
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Printer < TkObject
    extend TkCore

    TkCommandNames = ['::blt::printer'.freeze].freeze

    def self.enum(attribute)
      simplelist(tk_call('::blt::printer', 'enum', attribute))
    end

    def self.names(pat=None)
      simplelist(tk_call('::blt::printer', 'names', pat))
    end

    def self.open(printer)
      self.new(printer)
    end

    #################################

    def initialize(printer)
      @printer_id = tk_call('::blt::printer', 'open', printer)
    end

    def close
      tk_call('::blt::print', 'close', @printer_id)
      self
    end
    def get_attrs(var)
      tk_call('::blt::print', 'getattrs', @printer_id, var)
      var
    end
    def set_attrs(var)
      tk_call('::blt::print', 'setattrs', @printer_id, var)
      self
    end
    def snap(win)
      tk_call('::blt::print', 'snap', @printer_id, win)
      self
    end
    def write(str)
      tk_call('::blt::print', 'write', @printer_id, str)
      self
    end
    def write_with_title(title, str)
      tk_call('::blt::print', 'write', @printer_id, title, str)
      self
    end
  end
end
