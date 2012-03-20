#--
# sha2.rb - defines Digest::SHA2 class which wraps up the SHA256,
#           SHA384, and SHA512 classes.
#++
# Copyright (c) 2006 Akinori MUSHA <knu@iDaemons.org>
#
# All rights reserved.  You can redistribute and/or modify it under the same
# terms as Ruby.
#
#   $Id$

require 'digest'
require 'digest/sha2.so'

module Digest
  #
  # A meta digest provider class for SHA256, SHA384 and SHA512.
  #
  class SHA2 < Digest::Class
    # call-seq:
    #   Digest::SHA2.new(bitlen = 256) -> digest_obj
    #
    # Creates a new SHA2 hash object with a given bit length.
    #
    # Valid bit lengths are 256, 384 and 512.
    def initialize(bitlen = 256)
      case bitlen
      when 256
        @sha2 = Digest::SHA256.new
      when 384
        @sha2 = Digest::SHA384.new
      when 512
        @sha2 = Digest::SHA512.new
      else
        raise ArgumentError, "unsupported bit length: %s" % bitlen.inspect
      end
      @bitlen = bitlen
    end

    # call-seq:
    #   digest_obj.reset -> digest_obj
    #
    # Resets the digest to the initial state and returns self.
    def reset
      @sha2.reset
      self
    end

    # call-seq:
    #   digest_obj.update(string) -> digest_obj
    #   digest_obj << string -> digest_obj
    #
    # Updates the digest using a given _string_ and returns self.
    def update(str)
      @sha2.update(str)
      self
    end
    alias << update

    def finish # :nodoc:
      @sha2.digest!
    end
    private :finish


    # call-seq:
    #   digest_obj.block_length -> Integer
    #
    # Returns the block length of the digest in bytes.
    #
    #   Digest::SHA256.new.digest_length * 8
    #   # => 512
    #   Digest::SHA384.new.digest_length * 8
    #   # => 1024
    #   Digest::SHA512.new.digest_length * 8
    #   # => 1024
    def block_length
      @sha2.block_length
    end

    # call-seq:
    #   digest_obj.digest_length -> Integer
    #
    # Returns the length of the hash value of the digest in bytes.
    #
    #   Digest::SHA256.new.digest_length * 8
    #   # => 256
    #   Digest::SHA384.new.digest_length * 8
    #   # => 384
    #   Digest::SHA512.new.digest_length * 8
    #   # => 512
    #
    # For example, digests produced by Digest::SHA256 will always be 32 bytes
    # (256 bits) in size.
    def digest_length
      @sha2.digest_length
    end

    def initialize_copy(other) # :nodoc:
      @sha2 = other.instance_eval { @sha2.clone }
    end

    def inspect # :nodoc:
      "#<%s:%d %s>" % [self.class.name, @bitlen, hexdigest]
    end
  end
end
