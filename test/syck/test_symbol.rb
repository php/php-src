require 'test/unit'
require 'yaml'

module Syck
  class TestSymbol < Test::Unit::TestCase
    def test_to_yaml
      assert_equal :a, YAML.load(:a.to_yaml)
    end

    def test_dump
      assert_equal :a, YAML.load(YAML.dump(:a))
    end

    def test_stringy
      assert_equal :"1", YAML.load(YAML.dump(:"1"))
    end

    def test_load_quoted
      assert_equal :"1", YAML.load("--- :'1'\n")
    end
  end
end
