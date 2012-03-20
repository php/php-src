require_relative 'drbtest'

begin
  require 'drb/unix'
rescue LoadError
end

if Object.const_defined?("UNIXServer")


class DRbUNIXService < DRbService
  %w(ut_drb_drbunix.rb ut_array_drbunix.rb).each do |nm|
    add_service_command(nm)
  end

  uri = ARGV.shift if $0 == __FILE__
  @server = DRb::DRbServer.new(uri || 'drbunix:', self.manager, {})
end

class TestDRbUNIXCore < Test::Unit::TestCase
  include DRbCore
  def setup
    @ext = DRbUNIXService.ext_service('ut_drb_drbunix.rb')
    @there = @ext.front
  end

  def test_02_unknown
  end

  def test_01_02_loop
  end

  def test_05_eq
  end
end

class TestDRbUNIXAry < Test::Unit::TestCase
  include DRbAry
  def setup
    @ext = DRbUNIXService.ext_service('ut_array_drbunix.rb')
    @there = @ext.front
  end
end


end
