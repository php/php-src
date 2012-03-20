#--
#
# $RCSfile$
#
# = Ruby-space predefined Digest subclasses
# 
# = Info
# 'OpenSSL for Ruby 2' project
# Copyright (C) 2002  Michal Rokos <m.rokos@sh.cvut.cz>
# All rights reserved.
# 
# = Licence
# This program is licenced under the same licence as Ruby.
# (See the file 'LICENCE'.)
# 
# = Version
# $Id$
#
#++

module OpenSSL
  class Digest

    alg = %w(DSS DSS1 MD2 MD4 MD5 MDC2 RIPEMD160 SHA SHA1)
    if OPENSSL_VERSION_NUMBER > 0x00908000
      alg += %w(SHA224 SHA256 SHA384 SHA512)
    end

    # Return the +data+ hash computed with +name+ Digest. +name+ is either the
    # long name or short name of a supported digest algorithm.
    #
    # === Examples
    #
    #   OpenSSL::Digest.digest("SHA256, "abc")
    #
    # which is equivalent to:
    #
    #   OpenSSL::Digest::SHA256.digest("abc")

    def self.digest(name, data)
        super(data, name)
    end

    alg.each{|name|
      klass = Class.new(Digest){
        define_method(:initialize){|*data|
          if data.length > 1
            raise ArgumentError,
              "wrong number of arguments (#{data.length} for 1)"
          end
          super(name, data.first)
        }
      }
      singleton = (class << klass; self; end)
      singleton.class_eval{
        define_method(:digest){|data| Digest.digest(name, data) }
        define_method(:hexdigest){|data| Digest.hexdigest(name, data) }
      }
      const_set(name, klass)
    }

    # This class is only provided for backwards compatibility.  Use OpenSSL::Digest in the future.
    class Digest < Digest
      def initialize(*args)
        # add warning
        super(*args)
      end
    end

  end # Digest
end # OpenSSL

