# socket example - server side using thread
# usage: ruby tsvr.rb

require "socket"

gs = TCPServer.open(0)
addr = gs.addr
addr.shift
printf("server is on %s\n", addr.join(":"))

loop do
  Thread.start(gs.accept) do |s|
    print(s, " is accepted\n")
    while line = s.gets
      s.write(line)
    end
    print(s, " is gone\n")
    s.close
  end
end
