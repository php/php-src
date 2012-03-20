#
#   tk/console.rb : control the console on system without a real console
#
require 'tk'

module TkConsole
  include Tk
  extend Tk

  TkCommandNames = ['console'.freeze, 'consoleinterp'.freeze].freeze

  def self.create
    TkCore::INTERP._create_console
  end
  self.create  # initialize console

  def self.title(str=None)
    tk_call 'console', str
  end
  def self.hide
    tk_call_without_enc('console', 'hide')
  end
  def self.show
    tk_call_without_enc('console', 'show')
  end
  def self.eval(tcl_script)
    #
    # supports a Tcl script only
    # I have no idea to support a Ruby script seamlessly.
    #
    _fromUTF8(tk_call_without_enc('console', 'eval',
                                  _get_eval_enc_str(tcl_script)))
  end
  def self.maininterp_eval(tcl_script)
    #
    # supports a Tcl script only
    # I have no idea to support a Ruby script seamlessly.
    #
    _fromUTF8(tk_call_without_enc('consoleinterp', 'eval',
                                  _get_eval_enc_str(tcl_script)))

  end
  def self.maininterp_record(tcl_script)
    #
    # supports a Tcl script only
    # I have no idea to support a Ruby script seamlessly.
    #
    _fromUTF8(tk_call_without_enc('consoleinterp', 'record',
                                  _get_eval_enc_str(tcl_script)))

  end
end
