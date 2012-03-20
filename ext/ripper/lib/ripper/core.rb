#
# $Id$
#
# Copyright (c) 2003-2005 Minero Aoki
#
# This program is free software.
# You can distribute and/or modify this program under the Ruby License.
# For details of Ruby License, see ruby/COPYING.
#

require 'ripper.so'

class Ripper

  # Parses Ruby program read from _src_.
  # _src_ must be a String or a IO or a object which has #gets method.
  def Ripper.parse(src, filename = '(ripper)', lineno = 1)
    new(src, filename, lineno).parse
  end

  # This array contains name of parser events.
  PARSER_EVENTS = PARSER_EVENT_TABLE.keys

  # This array contains name of scanner events.
  SCANNER_EVENTS = SCANNER_EVENT_TABLE.keys

  # This array contains name of all ripper events.
  EVENTS = PARSER_EVENTS + SCANNER_EVENTS

  private

  #
  # Parser Events
  #

  PARSER_EVENT_TABLE.each do |id, arity|
    module_eval(<<-End, __FILE__, __LINE__ + 1)
      def on_#{id}(#{ ('a'..'z').to_a[0, arity].join(', ') })
        #{arity == 0 ? 'nil' : 'a'}
      end
    End
  end

  # This method is called when weak warning is produced by the parser.
  # _fmt_ and _args_ is printf style.
  def warn(fmt, *args)
  end

  # This method is called when strong warning is produced by the parser.
  # _fmt_ and _args_ is printf style.
  def warning(fmt, *args)
  end

  # This method is called when the parser found syntax error.
  def compile_error(msg)
  end

  #
  # Scanner Events
  #

  SCANNER_EVENTS.each do |id|
    module_eval(<<-End, __FILE__, __LINE__ + 1)
      def on_#{id}(token)
        token
      end
    End
  end

end
