# simple webpage fetcher

# The code demonstrates how a multi-protocol client should be written.
# TCPSocket is using getaddrinfo() internally, so there should be no problem.

require "socket"

if ARGV.size != 1
  STDERR.print "requires URL\n"
  exit
end

url = ARGV[0]
if url !~ /^http:\/\/([^\/]+)(\/.*)$/
  STDERR.print "only http with full hostname is supported\n"
  exit
end

# split URL into host, port and path
hostport = $1
path = $2
if (hostport =~ /^(.*):([0-9]+)$/)
  host = $1
  port = $2
else
  host = hostport
  port = 80
end
if host =~ /^\[(.*)\]$/
  host = $1
end

#STDERR.print "url=<#{ARGV[0]}>\n"
#STDERR.print "host=<#{host}>\n"
#STDERR.print "port=<#{port}>\n"
#STDERR.print "path=<#{path}>\n"

STDERR.print "conntecting to #{host} port #{port}\n"
c = TCPSocket.new(host, port)
dest = Socket.getnameinfo(c.getpeername,
		Socket::NI_NUMERICHOST|Socket::NI_NUMERICSERV)
STDERR.print "conntected to #{dest[0]} port #{dest[1]}\n"
c.print "GET #{path} HTTP/1.0\n"
c.print "Host: #{host}\n"
c.print "\n"
while c.gets
  print
end
