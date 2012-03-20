# simple httpd

# The code demonstrates how a multi-protocol daemon should be written.

require "socket"
require "thread"

port = 8888
res = Socket.getaddrinfo(nil, port, nil, Socket::SOCK_STREAM, nil, Socket::AI_PASSIVE)
sockpool = []
names = []
threads = []

res.each do |i|
  s = TCPServer.new(i[3], i[1])
  n = Socket.getnameinfo(s.getsockname, Socket::NI_NUMERICHOST|Socket::NI_NUMERICSERV).join(" port ")
  sockpool.push s
  names.push n
end

(0 .. sockpool.size - 1).each do |i|
  mysock = sockpool[i]
  myname = names[i]
  STDERR.print "socket #{mysock} started, address #{myname}\n"
  threads[i] = Thread.start do		# Thread.start cannot be used here!
    ls = mysock	# copy to dynamic variable
    t = Thread.current
    STDERR.print "socket #{myname} listener started, pid #{$$} thread #{t}\n"
    while true
      as = ls.accept
      Thread.start do
	STDERR.print "socket #{myname} accepted, thread ", Thread.current, "\n"
	s = as	# copy to dynamic variable
	str = ''
	while line = s.gets
	  break if line == "\r\n" or line == "\n"
	  str << line
	end
	STDERR.print "socket #{myname} got string\n"
	s.write("HTTP/1.0 200 OK\n")
	s.write("Content-type: text/plain\n\n")
	s.write("this is test: my name is #{myname}, you sent:\n")
	s.write("---start\n")
	s.write(str)
	s.write("---end\n")
	s.close
	STDERR.print "socket #{myname} processed, thread ", Thread.current, " terminating\n"
      end
    end
  end
end

for t in threads
  t.join
end
