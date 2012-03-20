#
#  tkextlib/blt/eps.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/canvas'
require 'tkextlib/blt.rb'

module Tk::BLT
  class EPS < TkcItem
    CItemTypeName = 'eps'.freeze
    CItemTypeToClass[CItemTypeName] = self
  end
end

class Tk::Canvas
  alias __BLT_EPS_item_strval_optkeys __item_strval_optkeys
  def __item_strval_optkeys(id)
    __BLT_EPS_item_strval_optkeys(id) + [
      'shadowcolor', 'title', 'titlecolor'
    ]
  end
  private :__item_strval_optkeys

  alias __BLT_EPS_item_boolval_optkeys __item_boolval_optkeys
  def __item_boolval_optkeys(id)
    __BLT_EPS_item_boolval_optkeys(id) + ['showimage']
  end
  private :__item_boolval_optkeys
end

