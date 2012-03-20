require 'minitest/autorun'
require 'tmpdir'
require_relative '../ruby/envutil'

class RakeIntegration < MiniTest::Unit::TestCase
  include Test::Unit::Assertions
  RAKE_LOADER = File.expand_path(
    File.join(
    File.dirname(__FILE__),
    '..',
    '..',
    'lib',
    'rake',
    'rake_test_loader.rb'))

  def test_with_rake_runner
    Dir.mktmpdir do |dir|
      filename = File.join dir, 'testing.rb'
      File.open(filename, 'wb') do |f|
        f.write <<-eotest
require 'test/unit'
raise 'loaded twice' if defined?(FooTest)
class FooTest; end
        eotest
      end

      args = %w{ -w } + [RAKE_LOADER, filename]
      bug3972 = "[ruby-core:32864]"
      status = assert_in_out_err(args, "",
                    /0 tests, 0 assertions, 0 failures, 0 errors, 0 skips/,
                    [], bug3972)
      assert_equal(true, status.success?, bug3972)
    end
  end
end
