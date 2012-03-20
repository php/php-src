require "webrick"
require "webrick/https"

hostname = WEBrick::Utils::getservername
subject = [["O", "ruby-lang.org"], ["OU", "sample"], ["CN", hostname]]
comment = "Comment for self-signed certificate"

httpd = WEBrick::HTTPServer.new(
  :DocumentRoot   => File::dirname(__FILE__),
  :Port           => 10443,
  :SSLEnable      => true,

  # Specify key pair and server certificate.
  # :SSLPrivateKey  => OpenSSL::PKey::RSA.new(File.read("server.key")),
  # :SSLCertificate => OpenSSL::X509::Certificate.new(File.read("server.crt")),

  # specify the following SSL options if you want to use auto
  # generated self-signed certificate.
  :SSLCertName    => subject,
  :SSLComment     => comment,

  :CGIPathEnv     => ENV["PATH"]  # PATH environment variable for CGI.
)

require "./hello"
httpd.mount("/hello", HelloServlet)

require "./demo-servlet"
httpd.mount("/urlencoded", DemoServlet, "application/x-www-form-urlencoded")
httpd.mount("/multipart", DemoServlet, "multipart/form-data")

trap(:INT){ httpd.shutdown }
httpd.start
