require 'test/unit'
require 'yaml'

module Syck
  ###
  # Test null from YAML spec:
  # http://yaml.org/type/null.html
  class TestNull < Test::Unit::TestCase
    def test_null_list
      assert_equal [nil] * 5, YAML.load(<<-eoyml)
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
