#--
# httpauth/authenticator.rb -- Authenticator mix-in module.
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: authenticator.rb,v 1.3 2003/02/20 07:15:47 gotoyuzo Exp $

module WEBrick
  module HTTPAuth

    ##
    # Module providing generic support for both Digest and Basic
    # authentication schemes.

    module Authenticator

      RequestField      = "Authorization"
      ResponseField     = "WWW-Authenticate"
      ResponseInfoField = "Authentication-Info"
      AuthException     = HTTPStatus::Unauthorized

      ##
      # Method of authentication, must be overridden by the including class

      AuthScheme        = nil

      ##
      # The realm this authenticator covers

      attr_reader :realm

      ##
      # The user database for this authenticator

      attr_reader :userdb

      ##
      # The logger for this authenticator

      attr_reader :logger

      private

      ##
      # Initializes the authenticator from +config+

      def check_init(config)
        [:UserDB, :Realm].each{|sym|
          unless config[sym]
            raise ArgumentError, "Argument #{sym.inspect} missing."
          end
        }
        @realm     = config[:Realm]
        @userdb    = config[:UserDB]
        @logger    = config[:Logger] || Log::new($stderr)
        @reload_db = config[:AutoReloadUserDB]
        @request_field   = self::class::RequestField
        @response_field  = self::class::ResponseField
        @resp_info_field = self::class::ResponseInfoField
        @auth_exception  = self::class::AuthException
        @auth_scheme     = self::class::AuthScheme
      end

      ##
      # Ensures +req+ has credentials that can be authenticated.

      def check_scheme(req)
        unless credentials = req[@request_field]
          error("no credentials in the request.")
          return nil
        end
        unless match = /^#{@auth_scheme}\s+/i.match(credentials)
          error("invalid scheme in %s.", credentials)
          info("%s: %s", @request_field, credentials) if $DEBUG
          return nil
        end
        return match.post_match
      end

      def log(meth, fmt, *args)
        msg = format("%s %s: ", @auth_scheme, @realm)
        msg << fmt % args
        @logger.send(meth, msg)
      end

      def error(fmt, *args)
        if @logger.error?
          log(:error, fmt, *args)
        end
      end

      def info(fmt, *args)
        if @logger.info?
          log(:info, fmt, *args)
        end
      end
    end

    ##
    # Module providing generic support for both Digest and Basic
    # authentication schemes for proxies.

    module ProxyAuthenticator
      RequestField  = "Proxy-Authorization"
      ResponseField = "Proxy-Authenticate"
      InfoField     = "Proxy-Authentication-Info"
      AuthException = HTTPStatus::ProxyAuthenticationRequired
    end
  end
end
