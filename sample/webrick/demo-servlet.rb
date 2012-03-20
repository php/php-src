require "webrick"
require "./demo-app"

class DemoServlet < WEBrick::HTTPServlet::AbstractServlet
  include DemoApplication
end
