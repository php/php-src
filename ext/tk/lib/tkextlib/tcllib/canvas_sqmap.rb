#
#  tkextlib/tcllib/canvas.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   *
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::canvas_sqmap', '0.2')
TkPackage.require('widget::canvas_sqmap')

module Tk::Tcllib
  module Widget
    class Canvas_Sqmap < Canvas
      TkCommandNames = ['::widget::canvas_sqmap'.freeze].freeze

      def image_set(cell, img)
        tk_send('image', 'set', cell, img)
        self
      end

      def image_unset(cell)
        tk_send('image', 'unset', cell)
        self
      end

      def flush
        tk_send('flush')
        self
      end
    end
  end
end
