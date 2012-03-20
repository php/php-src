require 'psych/helper'

module Psych
  class TestMergeKeys < TestCase
    def test_missing_merge_key
      yaml = <<-eoyml
bar:
  << : *foo
      eoyml
      exp = assert_raises(Psych::BadAlias) { Psych.load yaml }
      assert_match 'foo', exp.message
    end

    # [ruby-core:34679]
    def test_merge_key
      yaml = <<-eoyml
foo: &foo
  hello: world
bar:
  << : *foo
  baz: boo
      eoyml

      hash = {
        "foo" => { "hello" => "world"},
        "bar" => { "hello" => "world", "baz" => "boo" } }
      assert_equal hash, Psych.load(yaml)
    end

    def test_multiple_maps
      yaml = <<-eoyaml
---
- &CENTER { x: 1, y: 2 }
- &LEFT { x: 0, y: 2 }
- &BIG { r: 10 }
- &SMALL { r: 1 }

# All the following maps are equal:

- # Merge multiple maps
  << : [ *CENTER, *BIG ]
  label: center/big
      eoyaml

      hash = {
        'x' => 1,
        'y' => 2,
        'r' => 10,
        'label' => 'center/big'
      }

      assert_equal hash, Psych.load(yaml)[4]
    end

    def test_override
      yaml = <<-eoyaml
---
- &CENTER { x: 1, y: 2 }
- &LEFT { x: 0, y: 2 }
- &BIG { r: 10 }
- &SMALL { r: 1 }

# All the following maps are equal:

- # Override
  << : [ *BIG, *LEFT, *SMALL ]
  x: 1
  label: center/big
      eoyaml

      hash = {
        'x' => 1,
        'y' => 2,
        'r' => 10,
        'label' => 'center/big'
      }

      assert_equal hash, Psych.load(yaml)[4]
    end
  end
end
