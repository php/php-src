#
# httpauth/basicauth.rb -- HTTP basic access authentication
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: basicauth.rb,v 1.5 2003/02/20 07:15:47 gotoyuzo Exp $

require 'webrick/config'
require 'webrick/httpstatus'
require 'webrick/httpauth/authenticator'

module WEBrick
  module HTTPAuth

    ##
    # Basic Authentication for WEBrick
    #
    # Use this class to add basic authentication to a WEBrick servlet.
    #
    # Here is an example of how to set up a BasicAuth:
    #
    #   config = { :Realm => 'BasicAuth example realm' }
    #
    #   htpasswd = WEBrick::HTTPAuth::Htpasswd.new 'my_password_file'
    #   htpasswd.set_passwd config[:Realm], 'username', 'password'
    #   htpasswd.flush
    #
    #   config[:UserDB] = htpasswd
    #
    #   basic_auth = WEBrick::HTTPAuth::BasicAuth.new config

    class BasicAuth
      include Authenticator

      AuthScheme = "Basic"

      ##
      # Used by UserDB to create a basic password entry

      def self.make_passwd(realm, user, pass)
        pass ||= ""
        pass.crypt(Utils::random_string(2))
      end

      attr_reader :realm, :userdb, :logger

      ##
      # Creates a new BasicAuth instance.
      #
      # See WEBrick::Config::BasicAuth for default configuration entries
      #
      # You must supply the following configuration entries:
      #
      # :Realm:: The name of the realm being protected.
      # :UserDB:: A database of usernames and passwords.
      #           A WEBrick::HTTPAuth::Htpasswd instance should be used.

      def initialize(config, default=Config::BasicAuth)
        check_init(config)
        @config = default.dup.update(config)
      end

      ##
      # Authenticates a +req+ and returns a 401 Unauthorized using +res+ if
      # the authentication was not correct.

      def authenticate(req, res)
        unless basic_credentials = check_scheme(req)
          challenge(req, res)
        end
        userid, password = basic_credentials.unpack("m*")[0].split(":", 2)
        password ||= ""
        if userid.empty?
          error("user id was not given.")
          challenge(req, res)
        end
        unless encpass = @userdb.get_passwd(@realm, userid, @reload_db)
          error("%s: the user is not allowed.", userid)
          challenge(req, res)
        end
        if password.crypt(encpass) != encpass
          error("%s: password unmatch.", userid)
          challenge(req, res)
        end
        info("%s: authentication succeeded.", userid)
        req.user = userid
      end

      ##
      # Returns a challenge response which asks for for authentication
      # information

      def challenge(req, res)
        res[@response_field] = "#{@auth_scheme} realm=\"#{@realm}\""
        raise @auth_exception
      end
    end

    ##
    # Basic authentication for proxy servers.  See BasicAuth for details.

    class ProxyBasicAuth < BasicAuth
      include ProxyAuthenticator
    end
  end
end
