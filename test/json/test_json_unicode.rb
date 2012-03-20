#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

require 'test/unit'
require File.join(File.dirname(__FILE__), 'setup_variant')

class TC_JSONUnicode < Test::Unit::TestCase
  include JSON

  def test_unicode
    assert_equal '""', ''.to_json
    assert_equal '"\\b"', "\b".to_json
    assert_equal '"\u0001"', 0x1.chr.to_json
    assert_equal '"\u001f"', 0x1f.chr.to_json
    assert_equal '" "', ' '.to_json
    assert_equal "\"#{0x7f.chr}\"", 0x7f.chr.to_json
    utf8 = [ "© ≠ €! \01" ]
    json = '["© ≠ €! \u0001"]'
    assert_equal json, utf8.to_json(:ascii_only => false)
    assert_equal utf8, parse(json)
    json = '["\u00a9 \u2260 \u20ac! \u0001"]'
    assert_equal json, utf8.to_json(:ascii_only => true)
    assert_equal utf8, parse(json)
    utf8 = ["\343\201\202\343\201\204\343\201\206\343\201\210\343\201\212"]
    json = "[\"\343\201\202\343\201\204\343\201\206\343\201\210\343\201\212\"]"
    assert_equal utf8, parse(json)
    assert_equal json, utf8.to_json(:ascii_only => false)
    utf8 = ["\343\201\202\343\201\204\343\201\206\343\201\210\343\201\212"]
    assert_equal utf8, parse(json)
    json = "[\"\\u3042\\u3044\\u3046\\u3048\\u304a\"]"
    assert_equal json, utf8.to_json(:ascii_only => true)
    assert_equal utf8, parse(json)
    utf8 = ['საქართველო']
    json = '["საქართველო"]'
    assert_equal json, utf8.to_json(:ascii_only => false)
    json = "[\"\\u10e1\\u10d0\\u10e5\\u10d0\\u10e0\\u10d7\\u10d5\\u10d4\\u10da\\u10dd\"]"
    assert_equal json, utf8.to_json(:ascii_only => true)
    assert_equal utf8, parse(json)
    assert_equal '["Ã"]', JSON.generate(["Ã"], :ascii_only => false)
    assert_equal '["\\u00c3"]', JSON.generate(["Ã"], :ascii_only => true)
    assert_equal ["€"], JSON.parse('["\u20ac"]')
    utf8 = ["\xf0\xa0\x80\x81"]
    json = "[\"\xf0\xa0\x80\x81\"]"
    assert_equal json, JSON.generate(utf8, :ascii_only => false)
    assert_equal utf8, JSON.parse(json)
    json = '["\ud840\udc01"]'
    assert_equal json, JSON.generate(utf8, :ascii_only => true)
    assert_equal utf8, JSON.parse(json)
  end

  def test_chars
    (0..0x7f).each do |i|
      json = '["\u%04x"]' % i
      if RUBY_VERSION >= "1.9."
        i = i.chr
      end
      assert_equal i, JSON.parse(json).first[0]
      if i == ?\b
        generated = JSON.generate(["" << i])
        assert '["\b"]' == generated || '["\10"]' == generated
      elsif [?\n, ?\r, ?\t, ?\f].include?(i)
        assert_equal '[' << ('' << i).dump << ']', JSON.generate(["" << i])
      elsif i.chr < 0x20.chr
        assert_equal json, JSON.generate(["" << i])
      end
    end
    assert_raise(JSON::GeneratorError) do
      JSON.generate(["\x80"], :ascii_only => true)
    end
    assert_equal "\302\200", JSON.parse('["\u0080"]').first
  end
end
