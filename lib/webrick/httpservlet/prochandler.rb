#
# prochandler.rb -- ProcHandler Class
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: prochandler.rb,v 1.7 2002/09/21 12:23:42 gotoyuzo Exp $

require 'webrick/httpservlet/abstract.rb'

module WEBrick
  module HTTPServlet

    class ProcHandler < AbstractServlet
      def get_instance(server, *options)
        self
      end

      def initialize(proc)
        @proc = proc
      end

      def do_GET(request, response)
        @proc.call(request, response)
      end

      alias do_POST do_GET
    end

  end
end
