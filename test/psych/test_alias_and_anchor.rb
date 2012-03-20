require 'psych/helper'

module Psych
 class TestAliasAndAnchor < TestCase
   def test_mri_compatibility
     yaml = <<EOYAML
---
- &id001 !ruby/object {}

- *id001
- *id001
EOYAML
     result = Psych.load yaml
     result.each {|el| assert_same(result[0], el) }
   end

   def test_anchor_alias_round_trip
     o = Object.new
     original = [o,o,o]

     yaml = Psych.dump original
     result = Psych.load yaml
     result.each {|el| assert_same(result[0], el) }
   end
 end
end
