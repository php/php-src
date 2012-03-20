#!/usr/bin/env ruby

require 'openssl'
include OpenSSL::X509

def cert2text(cert_str)
  [Certificate, CRL, Request].each do |klass|
    begin
      puts klass.new(cert_str).to_text
      return
    rescue
    end
  end
  raise ArgumentError.new('Unknown format.')
end

if ARGV.empty?
  cert2text(STDIN.read)
else
  ARGV.each do |file|
    cert2text(File.read(file))
  end
end
