require "webrick"
require "webrick/httpproxy"

# The :ProxyContentHandler proc will be invoked before sending a response to
# the User-Agent.  You can inspect the pair of request and response messages
# (or edit the response message if necessary).

pch = Proc.new{|req, res|
  p [ req.request_line, res.status_line ]
}

def upstream_proxy
  if prx = ENV["http_proxy"]
    return URI.parse(prx)
  end
  return nil
end

httpd = WEBrick::HTTPProxyServer.new(
  :Port     => 10080,
  :ProxyContentHandler => pch,
  :ProxyURI => upstream_proxy
)
Signal.trap(:INT){ httpd.shutdown }
httpd.start
