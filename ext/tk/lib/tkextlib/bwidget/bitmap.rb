#
#  tkextlib/bwidget/bitmap.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tk/image'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class Bitmap < TkPhotoImage
    end
  end
end

class Tk::BWidget::Bitmap
  def initialize(name)
    @path = tk_call_without_enc('Bitmap::get', name)
    Tk_IMGTBL[@path] = self
  end
end
