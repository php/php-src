require "#{File.dirname(File.expand_path(__FILE__))}/ut_drb"

if __FILE__ == $0
  def ARGV.shift
    it = super()
    raise "usage: #{$0} <manager-uri> <name>" unless it
    it
  end

  DRb::DRbServer.default_argc_limit(8)
  DRb::DRbServer.default_load_limit(4096)
  DRb.start_service('drbunix:', DRbEx.new)
  es = DRb::ExtServ.new(ARGV.shift, ARGV.shift)
  DRb.thread.join
end

