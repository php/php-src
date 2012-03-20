#--
# htmlutils.rb -- HTMLUtils Module
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000, 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: htmlutils.rb,v 1.7 2002/09/21 12:23:35 gotoyuzo Exp $

module WEBrick
  module HTMLUtils

    ##
    # Escapes &, ", > and < in +string+

    def escape(string)
      str = string ? string.dup : ""
      str.gsub!(/&/n, '&amp;')
      str.gsub!(/\"/n, '&quot;')
      str.gsub!(/>/n, '&gt;')
      str.gsub!(/</n, '&lt;')
      str
    end
    module_function :escape

  end
end
