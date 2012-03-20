#!/usr/bin/env ruby
require "webrick/cgi"
require "webrick/https" # should load if it runs on HTTPS server
require "./demo-app"

class DemoCGI < WEBrick::CGI
  include DemoApplication
end

config = { :NPH => false }
cgi = DemoCGI.new(config, "multipart/form-data")
cgi.start
