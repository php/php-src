require 'psych/helper'

module Psych
  class TestOmap < TestCase
    def test_self_referential
      map = Psych::Omap.new
      map['foo'] = 'bar'
      map['self'] = map
      assert_equal(map, Psych.load(Psych.dump(map)))
    end

    def test_keys
      map = Psych::Omap.new
      map['foo'] = 'bar'
      assert_equal 'bar', map['foo']
    end

    def test_order
      map = Psych::Omap.new
      map['a'] = 'b'
      map['b'] = 'c'
      assert_equal [%w{a b}, %w{b c}], map.to_a
    end

    def test_square
      list = [["a", "b"], ["b", "c"]]
      map = Psych::Omap[*list.flatten]
      assert_equal list, map.to_a
      assert_equal 'b', map['a']
      assert_equal 'c', map['b']
    end

    def test_dump
      map = Psych::Omap['a', 'b', 'c', 'd']
      yaml = Psych.dump(map)
      assert_match('!omap', yaml)
      assert_match('- a: b', yaml)
      assert_match('- c: d', yaml)
    end

    def test_round_trip
      list = [["a", "b"], ["b", "c"]]
      map = Psych::Omap[*list.flatten]
      assert_cycle(map)
    end

    def test_load
      list = [["a", "b"], ["c", "d"]]
      map = Psych.load(<<-eoyml)
--- !omap
- a: b
- c: d
      eoyml
      assert_equal list, map.to_a
    end

    # NOTE: This test will not work with Syck
    def test_load_shorthand
      list = [["a", "b"], ["c", "d"]]
      map = Psych.load(<<-eoyml)
--- !!omap
- a: b
- c: d
      eoyml
      assert_equal list, map.to_a
    end
  end
end
