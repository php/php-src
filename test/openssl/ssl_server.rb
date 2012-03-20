require "socket"
require "thread"
require "openssl"
require File.join(File.dirname(__FILE__), "utils.rb")

def get_pem(io=$stdin)
  buf = ""
  while line = io.gets
    if /^-----BEGIN / =~ line
      buf << line
      break
    end
  end
  while line = io.gets
    buf << line
    if /^-----END / =~ line
      break
    end
  end
  return buf
end

def make_key(pem)
  begin
    return OpenSSL::PKey::RSA.new(pem)
  rescue
    return OpenSSL::PKey::DSA.new(pem)
  end
end

ca_cert  = OpenSSL::X509::Certificate.new(get_pem)
ssl_cert = OpenSSL::X509::Certificate.new(get_pem)
ssl_key  = make_key(get_pem)
port = Integer(ARGV.shift)
verify_mode = Integer(ARGV.shift)
start_immediately = (/yes/ =~ ARGV.shift)

store = OpenSSL::X509::Store.new
store.add_cert(ca_cert)
store.purpose = OpenSSL::X509::PURPOSE_SSL_CLIENT
ctx = OpenSSL::SSL::SSLContext.new
ctx.cert_store = store
#ctx.extra_chain_cert = [ ca_cert ]
ctx.cert = ssl_cert
ctx.key = ssl_key
ctx.verify_mode = verify_mode

Socket.do_not_reverse_lookup = true
tcps = nil
100.times{|i|
  begin
    tcps = TCPServer.new("0.0.0.0", port+i)
    port = port + i
    break
  rescue Errno::EADDRINUSE
    next
  end
}
ssls = OpenSSL::SSL::SSLServer.new(tcps, ctx)
ssls.start_immediately = start_immediately

$stdout.sync = true
$stdout.puts Process.pid
$stdout.puts port

loop do
  ssl = ssls.accept rescue next
  Thread.start{
    q = Queue.new
    th = Thread.start{ ssl.write(q.shift) while true }
    while line = ssl.gets
      if line =~ /^STARTTLS$/
        ssl.accept
        next
      end
      q.push(line)
    end
    th.kill if q.empty?
    ssl.close
  }
end
