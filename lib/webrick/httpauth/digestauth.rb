#
# httpauth/digestauth.rb -- HTTP digest access authentication
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2003 Internet Programming with Ruby writers.
# Copyright (c) 2003 H.M.
#
# The original implementation is provided by H.M.
#   URL: http://rwiki.jin.gr.jp/cgi-bin/rw-cgi.rb?cmd=view;name=
#        %C7%A7%BE%DA%B5%A1%C7%BD%A4%F2%B2%FE%C2%A4%A4%B7%A4%C6%A4%DF%A4%EB
#
# $IPR: digestauth.rb,v 1.5 2003/02/20 07:15:47 gotoyuzo Exp $

require 'webrick/config'
require 'webrick/httpstatus'
require 'webrick/httpauth/authenticator'
require 'digest/md5'
require 'digest/sha1'

module WEBrick
  module HTTPAuth

    ##
    # RFC 2617 Digest Access Authentication for WEBrick
    #
    # Use this class to add digest authentication to a WEBrick servlet.
    #
    # Here is an example of how to set up DigestAuth:
    #
    #   config = { :Realm => 'DigestAuth example realm' }
    #
    #   htdigest = WEBrick::HTTPAuth::Htdigest.new 'my_password_file'
    #   htdigest.set_passwd config[:Realm], 'username', 'password'
    #   htdigest.flush
    #
    #   config[:UserDB] = htdigest
    #
    #   digest_auth = WEBrick::HTTPAuth::DigestAuth.new config
    #
    # When using this as with a servlet be sure not to create a new DigestAuth
    # object in the servlet's #initialize.  By default WEBrick creates a new
    # servlet instance for every request and the DigestAuth object must be
    # used across requests.

    class DigestAuth
      include Authenticator

      AuthScheme = "Digest"
      OpaqueInfo = Struct.new(:time, :nonce, :nc)
      attr_reader :algorithm, :qop

      ##
      # Used by UserDB to create a digest password entry

      def self.make_passwd(realm, user, pass)
        pass ||= ""
        Digest::MD5::hexdigest([user, realm, pass].join(":"))
      end

      ##
      # Creates a new DigestAuth instance.  Be sure to use the same DigestAuth
      # instance for multiple requests as it saves state between requests in
      # order to perform authentication.
      #
      # See WEBrick::Config::DigestAuth for default configuration entries
      #
      # You must supply the following configuration entries:
      #
      # :Realm:: The name of the realm being protected.
      # :UserDB:: A database of usernames and passwords.
      #           A WEBrick::HTTPAuth::Htdigest instance should be used.

      def initialize(config, default=Config::DigestAuth)
        check_init(config)
        @config                 = default.dup.update(config)
        @algorithm              = @config[:Algorithm]
        @domain                 = @config[:Domain]
        @qop                    = @config[:Qop]
        @use_opaque             = @config[:UseOpaque]
        @use_next_nonce         = @config[:UseNextNonce]
        @check_nc               = @config[:CheckNc]
        @use_auth_info_header   = @config[:UseAuthenticationInfoHeader]
        @nonce_expire_period    = @config[:NonceExpirePeriod]
        @nonce_expire_delta     = @config[:NonceExpireDelta]
        @internet_explorer_hack = @config[:InternetExplorerHack]

        case @algorithm
        when 'MD5','MD5-sess'
          @h = Digest::MD5
        when 'SHA1','SHA1-sess'  # it is a bonus feature :-)
          @h = Digest::SHA1
        else
          msg = format('Algorithm "%s" is not supported.', @algorithm)
          raise ArgumentError.new(msg)
        end

        @instance_key = hexdigest(self.__id__, Time.now.to_i, Process.pid)
        @opaques = {}
        @last_nonce_expire = Time.now
        @mutex = Mutex.new
      end

      ##
      # Authenticates a +req+ and returns a 401 Unauthorized using +res+ if
      # the authentication was not correct.

      def authenticate(req, res)
        unless result = @mutex.synchronize{ _authenticate(req, res) }
          challenge(req, res)
        end
        if result == :nonce_is_stale
          challenge(req, res, true)
        end
        return true
      end

      ##
      # Returns a challenge response which asks for for authentication
      # information

      def challenge(req, res, stale=false)
        nonce = generate_next_nonce(req)
        if @use_opaque
          opaque = generate_opaque(req)
          @opaques[opaque].nonce = nonce
        end

        param = Hash.new
        param["realm"]  = HTTPUtils::quote(@realm)
        param["domain"] = HTTPUtils::quote(@domain.to_a.join(" ")) if @domain
        param["nonce"]  = HTTPUtils::quote(nonce)
        param["opaque"] = HTTPUtils::quote(opaque) if opaque
        param["stale"]  = stale.to_s
        param["algorithm"] = @algorithm
        param["qop"]    = HTTPUtils::quote(@qop.to_a.join(",")) if @qop

        res[@response_field] =
          "#{@auth_scheme} " + param.map{|k,v| "#{k}=#{v}" }.join(", ")
        info("%s: %s", @response_field, res[@response_field]) if $DEBUG
        raise @auth_exception
      end

      private

      MustParams = ['username','realm','nonce','uri','response']
      MustParamsAuth = ['cnonce','nc']

      def _authenticate(req, res)
        unless digest_credentials = check_scheme(req)
          return false
        end

        auth_req = split_param_value(digest_credentials)
        if auth_req['qop'] == "auth" || auth_req['qop'] == "auth-int"
          req_params = MustParams + MustParamsAuth
        else
          req_params = MustParams
        end
        req_params.each{|key|
          unless auth_req.has_key?(key)
            error('%s: parameter missing. "%s"', auth_req['username'], key)
            raise HTTPStatus::BadRequest
          end
        }

        if !check_uri(req, auth_req)
          raise HTTPStatus::BadRequest
        end

        if auth_req['realm'] != @realm
          error('%s: realm unmatch. "%s" for "%s"',
                auth_req['username'], auth_req['realm'], @realm)
          return false
        end

        auth_req['algorithm'] ||= 'MD5'
        if auth_req['algorithm'].upcase != @algorithm.upcase
          error('%s: algorithm unmatch. "%s" for "%s"',
                auth_req['username'], auth_req['algorithm'], @algorithm)
          return false
        end

        if (@qop.nil? && auth_req.has_key?('qop')) ||
           (@qop && (! @qop.member?(auth_req['qop'])))
          error('%s: the qop is not allowed. "%s"',
                auth_req['username'], auth_req['qop'])
          return false
        end

        password = @userdb.get_passwd(@realm, auth_req['username'], @reload_db)
        unless password
          error('%s: the user is not allowd.', auth_req['username'])
          return false
        end

        nonce_is_invalid = false
        if @use_opaque
          info("@opaque = %s", @opaque.inspect) if $DEBUG
          if !(opaque = auth_req['opaque'])
            error('%s: opaque is not given.', auth_req['username'])
            nonce_is_invalid = true
          elsif !(opaque_struct = @opaques[opaque])
            error('%s: invalid opaque is given.', auth_req['username'])
            nonce_is_invalid = true
          elsif !check_opaque(opaque_struct, req, auth_req)
            @opaques.delete(auth_req['opaque'])
            nonce_is_invalid = true
          end
        elsif !check_nonce(req, auth_req)
          nonce_is_invalid = true
        end

        if /-sess$/i =~ auth_req['algorithm']
          ha1 = hexdigest(password, auth_req['nonce'], auth_req['cnonce'])
        else
          ha1 = password
        end

        if auth_req['qop'] == "auth" || auth_req['qop'] == nil
          ha2 = hexdigest(req.request_method, auth_req['uri'])
          ha2_res = hexdigest("", auth_req['uri'])
        elsif auth_req['qop'] == "auth-int"
          ha2 = hexdigest(req.request_method, auth_req['uri'],
                          hexdigest(req.body))
          ha2_res = hexdigest("", auth_req['uri'], hexdigest(res.body))
        end

        if auth_req['qop'] == "auth" || auth_req['qop'] == "auth-int"
          param2 = ['nonce', 'nc', 'cnonce', 'qop'].map{|key|
            auth_req[key]
          }.join(':')
          digest     = hexdigest(ha1, param2, ha2)
          digest_res = hexdigest(ha1, param2, ha2_res)
        else
          digest     = hexdigest(ha1, auth_req['nonce'], ha2)
          digest_res = hexdigest(ha1, auth_req['nonce'], ha2_res)
        end

        if digest != auth_req['response']
          error("%s: digest unmatch.", auth_req['username'])
          return false
        elsif nonce_is_invalid
          error('%s: digest is valid, but nonce is not valid.',
                auth_req['username'])
          return :nonce_is_stale
        elsif @use_auth_info_header
          auth_info = {
            'nextnonce' => generate_next_nonce(req),
            'rspauth'   => digest_res
          }
          if @use_opaque
            opaque_struct.time  = req.request_time
            opaque_struct.nonce = auth_info['nextnonce']
            opaque_struct.nc    = "%08x" % (auth_req['nc'].hex + 1)
          end
          if auth_req['qop'] == "auth" || auth_req['qop'] == "auth-int"
            ['qop','cnonce','nc'].each{|key|
              auth_info[key] = auth_req[key]
            }
          end
          res[@resp_info_field] = auth_info.keys.map{|key|
            if key == 'nc'
              key + '=' + auth_info[key]
            else
              key + "=" + HTTPUtils::quote(auth_info[key])
            end
          }.join(', ')
        end
        info('%s: authentication succeeded.', auth_req['username'])
        req.user = auth_req['username']
        return true
      end

      def split_param_value(string)
        ret = {}
        while string.bytesize != 0
          case string
          when /^\s*([\w\-\.\*\%\!]+)=\s*\"((\\.|[^\"])*)\"\s*,?/
            key = $1
            matched = $2
            string = $'
            ret[key] = matched.gsub(/\\(.)/, "\\1")
          when /^\s*([\w\-\.\*\%\!]+)=\s*([^,\"]*),?/
            key = $1
            matched = $2
            string = $'
            ret[key] = matched.clone
          when /^s*^,/
            string = $'
          else
            break
          end
        end
        ret
      end

      def generate_next_nonce(req)
        now = "%012d" % req.request_time.to_i
        pk  = hexdigest(now, @instance_key)[0,32]
        nonce = [now + ":" + pk].pack("m*").chop # it has 60 length of chars.
        nonce
      end

      def check_nonce(req, auth_req)
        username = auth_req['username']
        nonce = auth_req['nonce']

        pub_time, pk = nonce.unpack("m*")[0].split(":", 2)
        if (!pub_time || !pk)
          error("%s: empty nonce is given", username)
          return false
        elsif (hexdigest(pub_time, @instance_key)[0,32] != pk)
          error("%s: invalid private-key: %s for %s",
                username, hexdigest(pub_time, @instance_key)[0,32], pk)
          return false
        end

        diff_time = req.request_time.to_i - pub_time.to_i
        if (diff_time < 0)
          error("%s: difference of time-stamp is negative.", username)
          return false
        elsif diff_time > @nonce_expire_period
          error("%s: nonce is expired.", username)
          return false
        end

        return true
      end

      def generate_opaque(req)
        @mutex.synchronize{
          now = req.request_time
          if now - @last_nonce_expire > @nonce_expire_delta
            @opaques.delete_if{|key,val|
              (now - val.time) > @nonce_expire_period
            }
            @last_nonce_expire = now
          end
          begin
            opaque = Utils::random_string(16)
          end while @opaques[opaque]
          @opaques[opaque] = OpaqueInfo.new(now, nil, '00000001')
          opaque
        }
      end

      def check_opaque(opaque_struct, req, auth_req)
        if (@use_next_nonce && auth_req['nonce'] != opaque_struct.nonce)
          error('%s: nonce unmatched. "%s" for "%s"',
                auth_req['username'], auth_req['nonce'], opaque_struct.nonce)
          return false
        elsif !check_nonce(req, auth_req)
          return false
        end
        if (@check_nc && auth_req['nc'] != opaque_struct.nc)
          error('%s: nc unmatched."%s" for "%s"',
                auth_req['username'], auth_req['nc'], opaque_struct.nc)
          return false
        end
        true
      end

      def check_uri(req, auth_req)
        uri = auth_req['uri']
        if uri != req.request_uri.to_s && uri != req.unparsed_uri &&
           (@internet_explorer_hack && uri != req.path)
          error('%s: uri unmatch. "%s" for "%s"', auth_req['username'],
                auth_req['uri'], req.request_uri.to_s)
          return false
        end
        true
      end

      def hexdigest(*args)
        @h.hexdigest(args.join(":"))
      end

    end

    ##
    # Digest authentication for proxy servers.  See DigestAuth for details.

    class ProxyDigestAuth < DigestAuth
      include ProxyAuthenticator

      private
      def check_uri(req, auth_req)
        return true
      end
    end
  end
end
