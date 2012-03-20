require 'psych/helper'

module Psych
  class TestObjectReferences < TestCase
    def test_range_has_references
      assert_reference_trip 1..2
    end

    def test_module_has_references
      assert_reference_trip Psych
    end

    def test_class_has_references
      assert_reference_trip TestObjectReferences
    end

    def test_rational_has_references
      assert_reference_trip Rational('1.2')
    end

    def test_complex_has_references
      assert_reference_trip Complex(1, 2)
    end

    def test_datetime_has_references
      assert_reference_trip DateTime.now
    end

    def assert_reference_trip obj
      yml = Psych.dump([obj, obj])
      assert_match(/\*-?\d+/, yml)
      data = Psych.load yml
      assert_equal data.first.object_id, data.last.object_id
    end

    def test_float_references
      data = Psych.load <<-eoyml
--- 
- &name 1.2
- *name
      eoyml
      assert_equal data.first, data.last
      assert_equal data.first.object_id, data.last.object_id
    end

    def test_binary_references
      data = Psych.load <<-eoyml
---
- &name !binary |-
  aGVsbG8gd29ybGQh
- *name
      eoyml
      assert_equal data.first, data.last
      assert_equal data.first.object_id, data.last.object_id
    end

    def test_regexp_references
      data = Psych.load <<-eoyml
--- 
- &name !ruby/regexp /pattern/i
- *name
      eoyml
      assert_equal data.first, data.last
      assert_equal data.first.object_id, data.last.object_id
    end
  end
end
