#
#  tkextlib/blt/winop.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module Winop
    extend TkCore

    TkCommandNames = ['::blt::winop'.freeze].freeze
  end
  WinOp = Winop
end

class << Tk::BLT::Winop
  def changes(win)
    tk_call('::blt::winop', 'changes', win)
  end

  def colormap(win)
    Hash[*list(tk_call('::blt::winop', 'colormap', win))]
  end

  def convolve(src, dest, filter)
    tk_call('::blt::winop', 'convolve', src, dest, filter)
  end

  def image_convolve(src, dest, filter)
    tk_call('::blt::winop', 'image', 'convolve', src, dest, filter)
  end
  def image_gradient(photo, left, right, type)
    tk_call('::blt::winop', 'image', 'gradient', photo, left, right, type)
  end
  def image_read_jpeg(file, photo)
    tk_call('::blt::winop', 'image', 'readjpeg', file, photo)
  end
  def image_resample(src, dest, horiz_filter=None, vert_filter=None)
    tk_call('::blt::winop', 'image', 'resample',
            src, dest, horiz_filter, vert_filter)
  end
  def image_rotate(src, dest, angle)
    tk_call('::blt::winop', 'image', 'rotate', src, dest, angle)
  end
  def image_snap(win, photo, width=None, height=None)
    tk_call('::blt::winop', 'image', 'snap', win, photo, width, height)
  end
  def image_subsample(src, dest, x, y, width, height,
                      horiz_filter=None, vert_filter=None)
    tk_call('::blt::winop', 'image', 'subsample',
            src, dest, x, y, width, height, horiz_filter, vert_filter)
  end

  def quantize(src, dest, colors)
    tk_call('::blt::winop', 'quantize', src, dest, colors)
  end

  def query()
    tk_call('::blt::winop', 'query')
  end

  def read_jpeg(file, photo)
    tk_call('::blt::winop', 'readjpeg', file, photo)
  end

  def resample(src, dest, horiz_filter=None, vert_filter=None)
    tk_call('::blt::winop', 'resample',
            src, dest, horiz_filter, vert_filter)
  end

  def subsample(src, dest, x, y, width, height,
                horiz_filter=None, vert_filter=None)
    tk_call('::blt::winop', 'subsample',
            src, dest, x, y, width, height, horiz_filter, vert_filter)
  end

  def raise(*wins)
    tk_call('::blt::winop', 'raise', *wins)
  end

  def lower(*wins)
    tk_call('::blt::winop', 'lower', *wins)
  end

  def map(*wins)
    tk_call('::blt::winop', 'map', *wins)
  end

  def unmap(*wins)
    tk_call('::blt::winop', 'unmap', *wins)
  end

  def move(win, x, y)
    tk_call('::blt::winop', 'move', win, x, y)
  end

  def snap(win, photo)
    tk_call('::blt::winop', 'snap', win, photo)
  end

  def warpto(win = None)
    tk_call('::blt::winop', 'warpto', win)
  end
  alias warp_to warpto
end
