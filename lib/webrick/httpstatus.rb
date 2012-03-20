#--
# httpstatus.rb -- HTTPStatus Class
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000, 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: httpstatus.rb,v 1.11 2003/03/24 20:18:55 gotoyuzo Exp $

module WEBrick

  ##
  # This module is used to manager HTTP status codes.
  #
  # See http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html for more
  # information.
  module HTTPStatus

    ##
    # Root of the HTTP status class hierarchy
    class Status < StandardError
      def initialize(*args) # :nodoc:
        args[0] = AccessLog.escape(args[0]) unless args.empty?
        super(*args)
      end
      class << self
        attr_reader :code, :reason_phrase # :nodoc:
      end
      
      # Returns the HTTP status code
      def code() self::class::code end
      
      # Returns the HTTP status description
      def reason_phrase() self::class::reason_phrase end
      
      alias to_i code # :nodoc:
    end

    # Root of the HTTP info statuses
    class Info        < Status; end
    # Root of the HTTP sucess statuses
    class Success     < Status; end
    # Root of the HTTP redirect statuses
    class Redirect    < Status; end
    # Root of the HTTP error statuses
    class Error       < Status; end
    # Root of the HTTP client error statuses
    class ClientError < Error; end
    # Root of the HTTP server error statuses
    class ServerError < Error; end

    class EOFError < StandardError; end

    # HTTP status codes and descriptions
    StatusMessage = { # :nodoc:
      100 => 'Continue',
      101 => 'Switching Protocols',
      200 => 'OK',
      201 => 'Created',
      202 => 'Accepted',
      203 => 'Non-Authoritative Information',
      204 => 'No Content',
      205 => 'Reset Content',
      206 => 'Partial Content',
      300 => 'Multiple Choices',
      301 => 'Moved Permanently',
      302 => 'Found',
      303 => 'See Other',
      304 => 'Not Modified',
      305 => 'Use Proxy',
      307 => 'Temporary Redirect',
      400 => 'Bad Request',
      401 => 'Unauthorized',
      402 => 'Payment Required',
      403 => 'Forbidden',
      404 => 'Not Found',
      405 => 'Method Not Allowed',
      406 => 'Not Acceptable',
      407 => 'Proxy Authentication Required',
      408 => 'Request Timeout',
      409 => 'Conflict',
      410 => 'Gone',
      411 => 'Length Required',
      412 => 'Precondition Failed',
      413 => 'Request Entity Too Large',
      414 => 'Request-URI Too Large',
      415 => 'Unsupported Media Type',
      416 => 'Request Range Not Satisfiable',
      417 => 'Expectation Failed',
      500 => 'Internal Server Error',
      501 => 'Not Implemented',
      502 => 'Bad Gateway',
      503 => 'Service Unavailable',
      504 => 'Gateway Timeout',
      505 => 'HTTP Version Not Supported'
    }

    # Maps a status code to the corresponding Status class
    CodeToError = {} # :nodoc:

    # Creates a status or error class for each status code and
    # populates the CodeToError map.
    StatusMessage.each{|code, message|
      message.freeze
      var_name = message.gsub(/[ \-]/,'_').upcase
      err_name = message.gsub(/[ \-]/,'')

      case code
      when 100...200; parent = Info
      when 200...300; parent = Success
      when 300...400; parent = Redirect
      when 400...500; parent = ClientError
      when 500...600; parent = ServerError
      end

      const_set("RC_#{var_name}", code)
      err_class = Class.new(parent)
      err_class.instance_variable_set(:@code, code)
      err_class.instance_variable_set(:@reason_phrase, message)
      const_set(err_name, err_class)
      CodeToError[code] = err_class
    }

    ##
    # Returns the description corresponding to the HTTP status +code+
    #
    #   WEBrick::HTTPStatus.reason_phrase 404
    #   => "Not Found"
    def reason_phrase(code)
      StatusMessage[code.to_i]
    end

    ##
    # Is +code+ an informational status?
    def info?(code)
      code.to_i >= 100 and code.to_i < 200
    end
    
    ##
    # Is +code+ a successful status?
    def success?(code)
      code.to_i >= 200 and code.to_i < 300
    end
    
    ##
    # Is +code+ a redirection status?
    def redirect?(code)
      code.to_i >= 300 and code.to_i < 400
    end
    
    ##
    # Is +code+ an error status?
    def error?(code)
      code.to_i >= 400 and code.to_i < 600
    end
    
    ##
    # Is +code+ a client error status?
    def client_error?(code)
      code.to_i >= 400 and code.to_i < 500
    end
    
    ##
    # Is +code+ a server error status?
    def server_error?(code)
      code.to_i >= 500 and code.to_i < 600
    end

    ##
    # Returns the status class corresponding to +code+
    #
    #   WEBrick::HTTPStatus[302]
    #   => WEBrick::HTTPStatus::NotFound
    #
    def self.[](code)
      CodeToError[code]
    end

    module_function :reason_phrase
    module_function :info?, :success?, :redirect?, :error?
    module_function :client_error?, :server_error?
  end
end
