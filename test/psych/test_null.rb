require 'psych/helper'

module Psych
  ###
  # Test null from YAML spec:
  # http://yaml.org/type/null.html
  class TestNull < TestCase
    def test_null_list
      assert_equal [nil] * 5, Psych.load(<<-eoyml)
---
- ~
- null
-
- Null
- NULL
      eoyml
    end
  end
end
