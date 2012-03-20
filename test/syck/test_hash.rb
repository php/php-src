require 'test/unit'
require 'yaml'

module Syck
  class TestHash < Test::Unit::TestCase
    def setup
      @hash = { :a => 'b' }
    end

    def test_to_yaml
      assert_equal @hash, YAML.load(@hash.to_yaml)
    end

    def test_dump
      assert_equal @hash, YAML.load(YAML.dump(@hash))
    end

    def test_ref_append
      hash = YAML.load(<<-eoyml)
---
foo: &foo
  hello: world
bar:
  <<: *foo
eoyml
      assert_equal({"foo"=>{"hello"=>"world"}, "bar"=>{"hello"=>"world"}}, hash)
    end
  end
end
