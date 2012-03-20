require 'test/unit'
require 'yaml'

module Syck
  class TestSet < Test::Unit::TestCase
    def setup
      @set = YAML::Set.new
      @set['foo'] = 'bar'
      @set['bar'] = 'baz'
    end

    def test_to_yaml
      assert_match(/!set/, @set.to_yaml)
    end

    def test_roundtrip
      assert_equal(@set, YAML.load(YAML.dump(@set)))
    end

    ###
    # FIXME: Syck should also support !!set as shorthand
    def test_load_from_yaml
      loaded = YAML.load(<<-eoyml)
--- !set
foo: bar
bar: baz
      eoyml
      assert_equal(@set, loaded)
    end
  end
end
