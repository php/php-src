# socket example - client side
# usage: ruby clnt.rb [host] port

require "socket"

if ARGV.length >= 2
  host = ARGV.shift
else
  host = "localhost"
end
print("Trying ", host, " ...")
STDOUT.flush
s = TCPSocket.open(host, ARGV.shift)
print(" done\n")
print("addr: ", s.addr.join(":"), "\n")
print("peer: ", s.peeraddr.join(":"), "\n")
while line = gets()
  s.write(line)
  print(s.readline)
end
s.close
