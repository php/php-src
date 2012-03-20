#!/usr/bin/env ruby

require 'drb'
require 'drb/ssl'

there = ARGV.shift || "drbssl://localhost:3456"

config = Hash.new
config[:SSLVerifyMode] = OpenSSL::SSL::VERIFY_PEER
config[:SSLVerifyCallback] = lambda{|ok,x509_store|
  p [ok, x509_store.error_string]
  true
}

DRb.start_service(nil,nil,config)
h = DRbObject.new(nil, there)
while line = gets
  p h.hello(line.chomp)
end
