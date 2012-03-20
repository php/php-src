require "webrick"

class HelloServlet < WEBrick::HTTPServlet::AbstractServlet
  def do_GET(req, res)
    res["content-type"] = "text/plain"
    res.body = "Hello, world.\n"
  end
end
