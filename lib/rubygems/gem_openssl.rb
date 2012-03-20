#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

#--
# Some system might not have OpenSSL installed, therefore the core
# library file openssl might not be available.  We localize testing
# for the presence of OpenSSL in this file.
#++

module Gem
  class << self
    ##
    # Is SSL (used by the signing commands) available on this
    # platform?

    def ssl_available?
      @ssl_available
    end

    ##
    # Is SSL available?

    attr_writer :ssl_available

    ##
    # Ensure that SSL is available.  Throw an exception if it is not.

    def ensure_ssl_available
      unless ssl_available?
        raise Gem::Exception, "SSL is not installed on this system"
      end
    end
  end
end

# :stopdoc:

begin
  require 'openssl'

  # Reference a constant defined in the .rb portion of ssl (just to
  # make sure that part is loaded too).

  Gem.ssl_available = !!OpenSSL::Digest::SHA1

  class OpenSSL::X509::Certificate
    # Check the validity of this certificate.
    def check_validity(issuer_cert = nil, time = Time.now)
      ret = if @not_before && @not_before > time
              [false, :expired, "not valid before '#@not_before'"]
            elsif @not_after && @not_after < time
              [false, :expired, "not valid after '#@not_after'"]
            elsif issuer_cert && !verify(issuer_cert.public_key)
              [false, :issuer, "#{issuer_cert.subject} is not issuer"]
            else
              [true, :ok, 'Valid certificate']
            end

      # return hash
      { :is_valid => ret[0], :error => ret[1], :desc => ret[2] }
    end
  end

rescue LoadError, StandardError
  Gem.ssl_available = false
end

module Gem::SSL

  # We make our own versions of the constants here.  This allows us
  # to reference the constants, even though some systems might not
  # have SSL installed in the Ruby core package.
  #
  # These constants are only used during load time.  At runtime, any
  # method that makes a direct reference to SSL software must be
  # protected with a Gem.ensure_ssl_available call.

  if Gem.ssl_available? then
    PKEY_RSA = OpenSSL::PKey::RSA
    DIGEST_SHA1 = OpenSSL::Digest::SHA1
  else
    PKEY_RSA = :rsa
    DIGEST_SHA1 = :sha1
  end

end

