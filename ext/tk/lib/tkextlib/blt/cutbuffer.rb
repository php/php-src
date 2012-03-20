#
#  tkextlib/blt/cutbuffer.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module CutBuffer
    TkCommandNames = ['::blt::cutbuffer'.freeze].freeze

    def self.get(num = 0)
      Tk.tk_call('::blt::cutbuffer', 'get', num)
    end
    def self.rotate(count = 1)
      Tk.tk_call('::blt::cutbuffer', 'rotate', count)
    end
    def self.set(val, num = 0)
      Tk.tk_call('::blt::cutbuffer', 'set', val, num)
    end
  end
end
