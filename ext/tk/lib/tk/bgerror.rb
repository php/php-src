#
#  tkbgerror -- bgerror ( tkerror ) module
#                     1998/07/16 by Hidetoshi Nagai <nagai@ai.kyutech.ac.jp>
#
require 'tk'

module TkBgError
  extend Tk

  TkCommandNames = ['bgerror'.freeze].freeze

  def bgerror(message)
    tk_call('bgerror', message)
  end
  alias tkerror bgerror
  alias show bgerror
  module_function :bgerror, :tkerror, :show

  def set_handler(hdlr = Proc.new) #==> handler :: proc{|msg| ...body... }
    tk_call('proc', 'bgerror', 'msg', install_cmd(hdlr) + ' $msg')
  end
  def set_default
    begin
      tk_call('rename', 'bgerror', '')
    rescue RuntimeError
    end
  end
  module_function :set_handler, :set_default
end
