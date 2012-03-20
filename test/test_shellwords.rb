# -*- coding: utf-8 -*-
require 'test/unit'
require 'shellwords'

class TestShellwords < Test::Unit::TestCase

  include Shellwords

  def setup
    @not_string = Class.new
    @cmd = "ruby my_prog.rb | less"
  end


  def test_string
    assert_instance_of(Array, shellwords(@cmd))
    assert_equal(4, shellwords(@cmd).length)
  end

  def test_unmatched_double_quote
    bad_cmd = 'one two "three'
    assert_raise ArgumentError do
      shellwords(bad_cmd)
    end
  end

  def test_unmatched_single_quote
    bad_cmd = "one two 'three"
    assert_raise ArgumentError do
      shellwords(bad_cmd)
    end
  end

  def test_unmatched_quotes
    bad_cmd = "one '"'"''""'""
    assert_raise ArgumentError do
      shellwords(bad_cmd)
    end
  end

  def test_backslashes
    cmdline, expected = [
      %q{/a//b///c////d/////e/ "/a//b///c////d/////e/ "'/a//b///c////d/////e/ '/a//b///c////d/////e/ },
      %q{a/b/c//d//e a/b/c//d//e /a//b///c////d/////e/ a/b/c//d//e }
    ].map { |str| str.tr("/", "\\\\") }

    assert_equal [expected], shellwords(cmdline)
  end

  def test_stringification
    assert_equal "3", shellescape(3)
    assert_equal "ps -p #{$$}", ['ps', '-p', $$].shelljoin
  end

  def test_multibyte_characters
    # This is not a spec.  It describes the current behavior which may
    # be changed in future.  There would be no multibyte character
    # used as shell meta-character that needs to be escaped.
    assert_equal "\\あ\\い", "あい".shellescape
  end
end
