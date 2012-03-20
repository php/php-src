require "#{File.dirname(File.expand_path(__FILE__))}/ut_drb"
require 'drb/ssl'

if __FILE__ == $0
  def ARGV.shift
    it = super()
    raise "usage: #{$0} <manager-uri> <name>" unless it
    it
  end

  config = Hash.new
  config[:SSLVerifyMode] = OpenSSL::SSL::VERIFY_PEER
  config[:SSLVerifyCallback] = lambda{|ok,x509_store|
    true
  }
  config[:SSLCertName] =
    [ ["C","JP"], ["O","Foo.DRuby.Org"], ["CN", "Sample"] ]

  DRb::DRbServer.default_argc_limit(8)
  DRb::DRbServer.default_load_limit(4096)
  DRb.start_service('drbssl://localhost:0', DRbEx.new, config)
  es = DRb::ExtServ.new(ARGV.shift, ARGV.shift)
  DRb.thread.join
end

