require_relative 'base'

class TestMkmf
  class TestHaveFramework < TestMkmf
    def test_core_foundation_framework
      assert(have_framework("CoreFoundation"), "try as Objective-C")
    end
  end
end if /darwin/ =~ RUBY_PLATFORM
