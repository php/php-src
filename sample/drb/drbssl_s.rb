#!/usr/bin/env ruby

require 'drb'
require 'drb/ssl'

here = ARGV.shift || "drbssl://localhost:3456"

class HelloWorld
  include DRbUndumped

  def hello(name)
    "Hello, #{name}."
  end
end

config = Hash.new
config[:verbose] = true
begin
  data = open("sample.key"){|io| io.read }
  config[:SSLPrivateKey] = OpenSSL::PKey::RSA.new(data)
  data = open("sample.crt"){|io| io.read }
  config[:SSLCertificate] = OpenSSL::X509::Certificate.new(data)
rescue
  $stderr.puts "Switching to use self-signed certificate"
  config[:SSLCertName] =
    [ ["C","JP"], ["O","Foo.DRuby.Org"], ["CN", "Sample"] ]
end

DRb.start_service(here, HelloWorld.new, config)
puts DRb.uri
DRb.thread.join
