# -*- ruby -*-
#
# tkencoding.rb
# written by ttate@jaist.ac.jp

class TclTkIp
  alias __eval _eval
  alias __invoke _invoke
  private :__eval
  private :__invoke

  attr_accessor :encoding

  def _eval(cmd)
    if( @encoding )
      _fromUTF8(__eval(_toUTF8(cmd,@encoding)),@encoding)
    else
      __eval(cmd)
    end
  end

  def _invoke(*cmds)
    if( @encoding )
      cmds = cmds.collect{|cmd| _toUTF8(cmd,@encoding)}
      _fromUTF8(__invoke(*cmds),@encoding)
    else
      __invoke(*cmds)
    end
  end
end

module Tk
  INTERP = TkCore::INTERP

  def encoding=(name)
    INTERP.encoding = name
  end

  def encoding
    INTERP.encoding
  end
end
