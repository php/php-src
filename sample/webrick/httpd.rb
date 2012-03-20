require "webrick"

httpd = WEBrick::HTTPServer.new(
  :DocumentRoot => File::dirname(__FILE__),
  :Port         => 10080,
  :Logger       => WEBrick::Log.new($stderr, WEBrick::Log::DEBUG),
  :AccessLog    => [
    [ $stderr, WEBrick::AccessLog::COMMON_LOG_FORMAT  ],
    [ $stderr, WEBrick::AccessLog::REFERER_LOG_FORMAT ],
    [ $stderr, WEBrick::AccessLog::AGENT_LOG_FORMAT   ],
  ],
  :CGIPathEnv   => ENV["PATH"]   # PATH environment variable for CGI.
)

require "./hello"
httpd.mount("/hello", HelloServlet)

require "./demo-servlet"
httpd.mount("/urlencoded", DemoServlet, "application/x-www-form-urlencoded")
httpd.mount("/multipart", DemoServlet, "multipart/form-data")

trap(:INT){ httpd.shutdown }
httpd.start
