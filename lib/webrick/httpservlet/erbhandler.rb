#
# erbhandler.rb -- ERBHandler Class
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: erbhandler.rb,v 1.25 2003/02/24 19:25:31 gotoyuzo Exp $

require 'webrick/httpservlet/abstract.rb'

require 'erb'

module WEBrick
  module HTTPServlet

    ##
    # ERBHandler evaluates an ERB file and returns the result.  This handler
    # is automatically used if there are .rhtml files in a directory served by
    # the FileHandler.
    #
    # ERBHandler supports GET and POST methods.
    #
    # The ERB file is evaluated with the local variables +servlet_request+ and
    # +servlet_response+ which are a WEBrick::HTTPRequest and
    # WEBrick::HTTPResponse respectively.
    #
    # Example .rhtml file:
    #
    #   Request to <%= servlet_request.request_uri %>
    #
    #   Query params <%= servlet_request.query.inspect %>

    class ERBHandler < AbstractServlet

      ##
      # Creates a new ERBHandler on +server+ that will evaluate and serve the
      # ERB file +name+

      def initialize(server, name)
        super(server, name)
        @script_filename = name
      end

      ##
      # Handles GET requests

      def do_GET(req, res)
        unless defined?(ERB)
          @logger.warn "#{self.class}: ERB not defined."
          raise HTTPStatus::Forbidden, "ERBHandler cannot work."
        end
        begin
          data = open(@script_filename){|io| io.read }
          res.body = evaluate(ERB.new(data), req, res)
          res['content-type'] ||=
            HTTPUtils::mime_type(@script_filename, @config[:MimeTypes])
        rescue StandardError => ex
          raise
        rescue Exception => ex
          @logger.error(ex)
          raise HTTPStatus::InternalServerError, ex.message
        end
      end

      ##
      # Handles POST requests

      alias do_POST do_GET

      private

      ##
      # Evaluates +erb+ providing +servlet_request+ and +servlet_response+ as
      # local variables.

      def evaluate(erb, servlet_request, servlet_response)
        Module.new.module_eval{
          servlet_request.meta_vars
          servlet_request.query
          erb.result(binding)
        }
      end
    end
  end
end
