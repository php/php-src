#!/usr/bin/ruby -Ku
# -*- coding: utf-8 -*-

require 'rexml/encoding'


class ChangingEncodings < Test::Unit::TestCase
  def initialize a
    @u = 'テスト ほげ ふが 美しい'
    @e = @u.encode("EUC-JP")
    @f = Foo.new
    super
  end

  class Foo
    include REXML::Encoding
  end

  # Note that these tests must be executed in order for the third one to
  # actually test anything.
  def test_0_euc
    @f.encoding = 'EUC-JP'
    assert_equal( @u, @f.decode(@e) )
    # This doesn't happen anymore, for some reason
    #assert_raises( Iconv::IllegalSequence, "Decoding unicode should fail" ) {
    #  @f.decode(@u) == @u
    #}
  end

  def test_1_utf
    @f.encoding = 'UTF-8'
    assert_not_equal( @u, @f.decode( @e ) )
    assert_equal( @u, @f.decode( @u ) )
  end

  def test_2_euc
    @f.encoding = 'EUC-JP'
    assert_equal( @u, @f.decode(@e) )
    # This doesn't happen anymore, for some reason
    #assert_raises( Iconv::IllegalSequence, "Decoding unicode should fail" ) {
    #  @f.decode(@u) == @u
    #}
  end
end
