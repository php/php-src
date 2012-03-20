#
# httpservlet.rb -- HTTPServlet Module
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: abstract.rb,v 1.24 2003/07/11 11:16:46 gotoyuzo Exp $

require 'thread'

require 'webrick/htmlutils'
require 'webrick/httputils'
require 'webrick/httpstatus'

module WEBrick
  module HTTPServlet
    class HTTPServletError < StandardError; end

    ##
    # AbstractServlet allows HTTP server modules to be reused across multiple
    # servers and allows encapsulation of functionality.
    #
    # By default a servlet will respond to GET, HEAD (through an alias to GET)
    # and OPTIONS requests.
    #
    # By default a new servlet is initialized for every request.  A servlet
    # instance can be reused by overriding ::get_instance in the
    # AbstractServlet subclass.
    #
    # == A Simple Servlet
    #
    #  class Simple < WEBrick::HTTPServlet::AbstractServlet
    #    def do_GET request, response
    #      status, content_type, body = do_stuff_with request
    #
    #      response.status = status
    #      response['Content-Type'] = content_type
    #      response.body = body
    #    end
    #
    #    def do_stuff_with request
    #      return 200, 'text/plain', 'you got a page'
    #    end
    #  end
    #
    # This servlet can be mounted on a server at a given path:
    #
    #   server.mount '/simple', Simple
    #
    # == Servlet Configuration
    #
    # Servlets can be configured via initialize.  The first argument is the
    # HTTP server the servlet is being initialized for.
    #
    #  class Configureable < Simple
    #    def initialize server, color, size
    #      super server
    #      @color = color
    #      @size = size
    #    end
    #
    #    def do_stuff_with request
    #      content = "<p " \
    #                %q{style="color: #{@color}; font-size: #{@size}"} \
    #                ">Hello, World!"
    #
    #      return 200, "text/html", content
    #    end
    #  end
    #
    # This servlet must be provided two arguments at mount time:
    #
    #   server.mount '/configurable', Configurable, 'red', '2em'

    class AbstractServlet

      ##
      # Factory for servlet instances that will handle a request from +server+
      # using +options+ from the mount point.  By default a new servlet
      # instance is created for every call.

      def self.get_instance(server, *options)
        self.new(server, *options)
      end

      ##
      # Initializes a new servlet for +server+ using +options+ which are
      # stored as-is in +@options+.  +@logger+ is also provided.

      def initialize(server, *options)
        @server = @config = server
        @logger = @server[:Logger]
        @options = options
      end

      ##
      # Dispatches to a +do_+ method based on +req+ if such a method is
      # available.  (+do_GET+ for a GET request).  Raises a MethodNotAllowed
      # exception if the method is not implemented.

      def service(req, res)
        method_name = "do_" + req.request_method.gsub(/-/, "_")
        if respond_to?(method_name)
          __send__(method_name, req, res)
        else
          raise HTTPStatus::MethodNotAllowed,
                "unsupported method `#{req.request_method}'."
        end
      end

      ##
      # Raises a NotFound exception

      def do_GET(req, res)
        raise HTTPStatus::NotFound, "not found."
      end

      ##
      # Dispatches to do_GET

      def do_HEAD(req, res)
        do_GET(req, res)
      end

      ##
      # Returns the allowed HTTP request methods

      def do_OPTIONS(req, res)
        m = self.methods.grep(/\Ado_([A-Z]+)\z/) {$1}
        m.sort!
        res["allow"] = m.join(",")
      end

      private

      ##
      # Redirects to a path ending in /

      def redirect_to_directory_uri(req, res)
        if req.path[-1] != ?/
          location = WEBrick::HTTPUtils.escape_path(req.path + "/")
          if req.query_string && req.query_string.bytesize > 0
            location << "?" << req.query_string
          end
          res.set_redirect(HTTPStatus::MovedPermanently, location)
        end
      end
    end

  end
end
