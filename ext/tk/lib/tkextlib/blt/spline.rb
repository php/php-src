#
#  tkextlib/blt/spline.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module Spline
    extend TkCore

    TkCommandNames = ['::blt::spline'.freeze].freeze

    def self.natural(x, y, sx, sy)
      tk_call('::blt::spline', 'natural', x, y, sx, sy)
    end

    def self.quadratic(x, y, sx, sy)
      tk_call('::blt::spline', 'quadratic', x, y, sx, sy)
    end
  end
end
