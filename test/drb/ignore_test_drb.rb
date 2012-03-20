require 'drbtest'

class TestDRbReusePort < Test::Unit::TestCase
  include DRbAry

  def setup
    @ext = DRbService.ext_service('ut_port.rb')
    @there = @ext.front
  end

  def teardown
    return unless @ext
    @ext.stop_service
    while true
      sleep 0.1
      begin
        @ext.alive?
      rescue DRb::DRbConnError
        break
      end
    end
  end
end

