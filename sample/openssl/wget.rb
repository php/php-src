#!/usr/bin/env ruby

require 'net/https'
require 'optparse'

options = ARGV.getopts('C:')

cert_store = options["C"]

uri = URI.parse(ARGV[0])
if proxy = ENV['HTTP_PROXY']
  prx_uri = URI.parse(proxy)
  prx_host = prx_uri.host
  prx_port = prx_uri.port
end

h = Net::HTTP.new(uri.host, uri.port, prx_host, prx_port)
h.set_debug_output($stderr) if $DEBUG
if uri.scheme == "https"
  h.use_ssl = true
  if cert_store
    if File.directory?(cert_store)
      h.ca_path = cert_store
    else
      h.ca_file = cert_store
    end
  end
end

path = uri.path.empty? ? "/" : uri.path
h.get2(path){|resp|
  STDERR.puts h.peer_cert.inspect if h.peer_cert
  print resp.body
}
