#!/usr/bin/env ruby

require 'socket'
require 'openssl'
require 'optparse'

options = ARGV.getopts("p:c:k:C:")

host      = ARGV[0] || "localhost"
port      = options["p"] || "2000"
cert_file = options["c"]
key_file  = options["k"]
ca_path   = options["C"]

ctx = OpenSSL::SSL::SSLContext.new()
if cert_file && key_file
  ctx.cert = OpenSSL::X509::Certificate.new(File::read(cert_file))
  ctx.key  = OpenSSL::PKey::RSA.new(File::read(key_file))
end
if ca_path
  ctx.verify_mode = OpenSSL::SSL::VERIFY_PEER
  ctx.ca_path = ca_path
else
  $stderr.puts "!!! WARNING: PEER CERTIFICATE WON'T BE VERIFIED !!!"
end

s = TCPSocket.new(host, port)
ssl = OpenSSL::SSL::SSLSocket.new(s, ctx)
ssl.connect # start SSL session
p ssl.peer_cert
errors = Hash.new
OpenSSL::X509.constants.grep(/^V_(ERR_|OK)/).each do |name|
  errors[OpenSSL::X509.const_get(name)] = name
end
p errors[ssl.verify_result]

ssl.sync_close = true  # if true the underlying socket will be
                       # closed in SSLSocket#close. (default: false)
while line = $stdin.gets
  ssl.write line
  puts ssl.gets.inspect
end

ssl.close
