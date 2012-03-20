require 'test/unit'
require 'yaml'

module Syck
  class TestArray < Test::Unit::TestCase
    def setup
      @current_engine = YAML::ENGINE.yamler
      YAML::ENGINE.yamler = 'syck'
      @list = [{ :a => 'b' }, 'foo']
    end

    def teardown
      YAML::ENGINE.yamler = @current_engine
    end

    def test_to_yaml
      assert_equal @list, YAML.load(@list.to_yaml)
    end

    def test_dump
      assert_equal @list, YAML.load(YAML.dump(@list))
    end
  end
end
