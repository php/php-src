require 'test/unit'

class TestFuncall < Test::Unit::TestCase
  module Relay
    def self.target(*args, &block)
      yield(*args) if block
    end
  end
  require '-test-/funcall/funcall'

  def test_with_funcall2
    ok = nil
    Relay.with_funcall2("feature#4504") {|arg| ok = arg || true}
    assert_nil(ok)
  end

  def test_with_funcall_passing_block
    ok = nil
    Relay.with_funcall_passing_block("feature#4504") {|arg| ok = arg || true}
    assert_equal("feature#4504", ok)
  end
end
