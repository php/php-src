#
# httpauth.rb -- HTTP access authentication
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000, 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: httpauth.rb,v 1.14 2003/07/22 19:20:42 gotoyuzo Exp $

require 'webrick/httpauth/basicauth'
require 'webrick/httpauth/digestauth'
require 'webrick/httpauth/htpasswd'
require 'webrick/httpauth/htdigest'
require 'webrick/httpauth/htgroup'

module WEBrick

  ##
  # HTTPAuth provides both basic and digest authentication.
  #
  # To enable authentication for requests in WEBrick you will need a user
  # database and an authenticator.  To start, here's an Htpasswd database for
  # use with a DigestAuth authenticator:
  #
  #   config = { :Realm => 'DigestAuth example realm' }
  #
  #   htpasswd = WEBrick::HTTPAuth::Htpasswd.new 'my_password_file'
  #   htpasswd.auth_type = WEBrick::HTTPAuth::DigestAuth
  #   htpasswd.set_passwd config[:Realm], 'username', 'password'
  #   htpasswd.flush
  #
  # The +:Realm+ is used to provide different access to different groups
  # across several resources on a server.  Typically you'll need only one
  # realm for a server.
  #
  # This database can be used to create an authenticator:
  #
  #   config[:UserDB] = htpasswd
  #
  #   digest_auth = WEBrick::HTTPAuth::DigestAuth.new config
  #
  # To authenticate a request call #authenticate with a request and response
  # object in a servlet:
  #
  #   def do_GET req, res
  #     @authenticator.authenticate req, res
  #   end
  #
  # For digest authentication the authenticator must not be created every
  # request, it must be passed in as an option via WEBrick::HTTPServer#mount.

  module HTTPAuth
    module_function

    def _basic_auth(req, res, realm, req_field, res_field, err_type,
                    block) # :nodoc:
      user = pass = nil
      if /^Basic\s+(.*)/o =~ req[req_field]
        userpass = $1
        user, pass = userpass.unpack("m*")[0].split(":", 2)
      end
      if block.call(user, pass)
        req.user = user
        return
      end
      res[res_field] = "Basic realm=\"#{realm}\""
      raise err_type
    end

    ##
    # Simple wrapper for providing basic authentication for a request.  When
    # called with a request +req+, response +res+, authentication +realm+ and
    # +block+ the block will be called with a +username+ and +password+.  If
    # the block returns true the request is allowed to continue, otherwise an
    # HTTPStatus::Unauthorized error is raised.

    def basic_auth(req, res, realm, &block) # :yield: username, password
      _basic_auth(req, res, realm, "Authorization", "WWW-Authenticate",
                  HTTPStatus::Unauthorized, block)
    end

    ##
    # Simple wrapper for providing basic authentication for a proxied request.
    # When called with a request +req+, response +res+, authentication +realm+
    # and +block+ the block will be called with a +username+ and +password+.
    # If the block returns true the request is allowed to continue, otherwise
    # an HTTPStatus::ProxyAuthenticationRequired error is raised.

    def proxy_basic_auth(req, res, realm, &block) # :yield: username, password
      _basic_auth(req, res, realm, "Proxy-Authorization", "Proxy-Authenticate",
                  HTTPStatus::ProxyAuthenticationRequired, block)
    end
  end
end
