#!/usr/bin/env ruby
#
# $RoughId: test.rb,v 1.4 2001/07/13 15:38:27 knu Exp $
# $Id$

require 'test/unit'

require 'digest'
%w[digest/md5 digest/rmd160 digest/sha1 digest/sha2].each do |lib|
  begin
    require lib
  rescue LoadError
  end
end

module TestDigest
  Data1 = "abc"
  Data2 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"

  def test_s_new
    self.class::DATA.each do |str, hexdigest|
      assert_raise(ArgumentError) { self.class::ALGO.new("") }
    end
  end

  def test_s_hexdigest
    self.class::DATA.each do |str, hexdigest|
      assert_equal(hexdigest, self.class::ALGO.hexdigest(str))
    end
  end

  def test_s_base64digest
    self.class::DATA.each do |str, hexdigest|
      digest = [hexdigest].pack("H*")
      assert_equal([digest].pack("m0"), self.class::ALGO.base64digest(str))
    end
  end

  def test_s_digest
    self.class::DATA.each do |str, hexdigest|
      digest = [hexdigest].pack("H*")
      assert_equal(digest, self.class::ALGO.digest(str))
    end
  end

  def test_update
    # This test is also for digest() and hexdigest()

    str = "ABC"

    md = self.class::ALGO.new
    md.update str
    assert_equal(self.class::ALGO.hexdigest(str), md.hexdigest)
    assert_equal(self.class::ALGO.digest(str), md.digest)
  end

  def test_eq
    # This test is also for clone()

    md1 = self.class::ALGO.new
    md1 << "ABC"

    assert_equal(md1, md1.clone, self.class::ALGO)

    md2 = self.class::ALGO.new
    md2 << "A"

    assert_not_equal(md1, md2, self.class::ALGO)

    md2 << "BC"

    assert_equal(md1, md2, self.class::ALGO)
  end

  def test_instance_eval
    assert_nothing_raised {
      self.class::ALGO.new.instance_eval { update "a" }
    }
  end

  def test_alignment
    md = self.class::ALGO.new
    assert_nothing_raised('#4320') {
      md.update('a' * 97)
      md.update('a' * 97)
      md.hexdigest
    }
  end

  class TestMD5 < Test::Unit::TestCase
    include TestDigest
    ALGO = Digest::MD5
    DATA = {
      Data1 => "900150983cd24fb0d6963f7d28e17f72",
      Data2 => "8215ef0796a20bcaaae116d3876c664a",
    }
  end if defined?(Digest::MD5)

  class TestSHA1 < Test::Unit::TestCase
    include TestDigest
    ALGO = Digest::SHA1
    DATA = {
      Data1 => "a9993e364706816aba3e25717850c26c9cd0d89d",
      Data2 => "84983e441c3bd26ebaae4aa1f95129e5e54670f1",
    }
  end if defined?(Digest::SHA1)

  class TestSHA256 < Test::Unit::TestCase
    include TestDigest
    ALGO = Digest::SHA256
    DATA = {
      Data1 => "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
      Data2 => "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
    }
  end if defined?(Digest::SHA256)

  class TestSHA384 < Test::Unit::TestCase
    include TestDigest
    ALGO = Digest::SHA384
    DATA = {
      Data1 => "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7",
      Data2 => "3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05abfe8f450de5f36bc6b0455a8520bc4e6f5fe95b1fe3c8452b",
    }
  end if defined?(Digest::SHA384)

  class TestSHA512 < Test::Unit::TestCase
    include TestDigest
    ALGO = Digest::SHA512
    DATA = {
      Data1 => "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
      Data2 => "204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445",
    }
  end if defined?(Digest::SHA512)

  class TestRMD160 < Test::Unit::TestCase
    include TestDigest
    ALGO = Digest::RMD160
    DATA = {
      Data1 => "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc",
      Data2 => "12a053384a9c0c88e405a06c27dcf49ada62eb2b",
    }
  end if defined?(Digest::RMD160)

  class TestBase < Test::Unit::TestCase
    def test_base
      bug3810 = '[ruby-core:32231]'
      assert_raise(NotImplementedError, bug3810) {Digest::Base.new}
    end
  end
end
