##
# = WEB server toolkit.
#
# WEBrick is an HTTP server toolkit that can be configured as an HTTPS server,
# a proxy server, and a virtual-host server.  WEBrick features complete
# logging of both server operations and HTTP access.  WEBrick supports both
# basic and digest authentication in addition to algorithms not in RFC 2617.
#
# A WEBrick servers can be composed of multiple WEBrick servers or servlets to
# provide differing behavior on a per-host or per-path basis.  WEBrick
# includes servlets for handling CGI scripts, ERb pages, ruby blocks and
# directory listings.
#
# WEBrick also includes tools for daemonizing a process and starting a process
# at a higher privilege level and dropping permissions.
#
# == Starting an HTTP server
#
# To create a new WEBrick::HTTPServer that will listen to connections on port
# 8000 and serve documents from the current user's public_html folder:
#
#   require 'webrick'
#
#   root = File.expand_path '~/public_html'
#   server = WEBrick::HTTPServer.new :Port => 8000, :DocumentRoot => root
#
# To run the server you will need to provide a suitable shutdown hook as
# starting the server blocks the current thread:
#
#   trap 'INT' do server.shutdown end
#
#   server.start
#
# == Custom Behavior
#
# The easiest way to have a server perform custom operations is through
# WEBrick::HTTPServer#mount_proc.  The block given will be called with a
# WEBrick::HTTPRequest with request info and a WEBrick::HTTPResponse which
# must be filled in appropriately:
#
#   server.mount_proc '/' do |req, res|
#     res.body = 'Hello, world!'
#   end
#
# Remember that <tt>server.mount_proc</tt> must <tt>server.start</tt>.
#
# == Servlets
#
# Advanced custom behavior can be obtained through mounting a subclass of
# WEBrick::HTTPServlet::AbstractServlet.  Servlets provide more modularity
# when writing an HTTP server than mount_proc allows.  Here is a simple
# servlet:
#
#   class Simple < WEBrick::HTTPServlet::AbstractServlet
#     def do_GET request, response
#       status, content_type, body = do_stuff_with request
#
#       response.status = 200
#       response['Content-Type'] = 'text/plain'
#       response.body = 'Hello, World!'
#     end
#   end
#
# To initialize the servlet you mount it on the server:
#
#   server.mount '/simple', Simple
#
# See WEBrick::HTTPServlet::AbstractServlet for more details.
#
# == Virtual Hosts
#
# A server can act as a virtual host for multiple host names.  After creating
# the listening host, additional hosts that do not listen can be created and
# attached as virtual hosts:
#
#   server = WEBrick::HTTPServer.new # ...
#
#   vhost = WEBrick::HTTPServer.new :ServerName => 'vhost.example',
#                                   :DoNotListen => true, # ...
#   vhost.mount '/', ...
#
#   server.virtual_host vhost
#
# If no +:DocumentRoot+ is provided and no servlets or procs are mounted on the
# main server it will return 404 for all URLs.
#
# == HTTPS
#
# To create an HTTPS server you only need to enable SSL and provide an SSL
# certificate name:
#
#   require 'webrick'
#   require 'webrick/https'
#
#   cert_name = [
#     %w[CN localhost],
#   ]
#
#   server = WEBrick::HTTPServer.new(:Port => 8000,
#                                    :SSLEnable => true,
#                                    :SSLCertName => cert_name)
#
# This will start the server with a self-generated self-signed certificate.
# The certificate will be changed every time the server is restarted.
#
# To create a server with a pre-determined key and certificate you can provide
# them:
#
#   require 'webrick'
#   require 'webrick/https'
#   require 'openssl'
#
#   cert = OpenSSL::X509::Certificate.new File.read '/path/to/cert.pem'
#   pkey = OpenSSL::PKey::RSA.new File.read '/path/to/pkey.pem'
#
#   server = WEBrick::HTTPServer.new(:Port => 8000,
#                                    :SSLEnable => true,
#                                    :SSLCertificate => cert,
#                                    :SSLPrivateKey => pkey)
#
# == Proxy Server
#
# WEBrick can act as a proxy server:
#
#   require 'webrick'
#   require 'webrick/httpproxy'
#
#   proxy = WEBrick::HTTPProxyServer.new :Port => 8000
#
#   trap 'INT' do proxy.shutdown end
#
# See WEBrick::HTTPProxy for further details including modifying proxied
# responses.
#
# == Basic and Digest authentication
#
# WEBrick provides both Basic and Digest authentication for regular and proxy
# servers.  See WEBrick::HTTPAuth, WEBrick::HTTPAuth::BasicAuth and
# WEBrick::HTTPAuth::DigestAuth.
#
# == WEBrick as a Production Web Server
#
# WEBrick can be run as a production server for small loads.
#
# === Daemonizing
#
# To start a WEBrick server as a daemon simple run WEBrick::Daemon.start
# before starting the server.
#
# === Dropping Permissions
#
# WEBrick can be started as one user to gain permission to bind to port 80 or
# 443 for serving HTTP or HTTPS traffic then can drop these permissions for
# regular operation.  To listen on all interfaces for HTTP traffic:
#
#   sockets = WEBrick::Utils.create_listeners nil, 80
#
# Then drop privileges:
#
#   WEBrick::Utils.su 'www'
#
# Then create a server that does not listen by default:
#
#   server = WEBrick::HTTPServer.new :DoNotListen => true, # ...
#
# Then overwrite the listening sockets with the port 80 sockets:
#
#   server.listeners.replace sockets
#
# === Logging
#
# WEBrick can separately log server operations and end-user access.  For
# server operations:
#
#   log_file = File.open '/var/log/webrick.log', 'a+'
#   log = WEBrick::Log.new log_file
#
# For user access logging:
#
#   access_log = [
#     [log_file, WEBrick::AccessLog::COMBINED_LOG_FORMAT],
#   ]
#
#   server = WEBrick::HTTPServer.new :Logger => log, :AccessLog => access_log
#
# See WEBrick::AccessLog for further log formats.
#
# === Log Rotation
#
# To rotate logs in WEBrick on a HUP signal (like syslogd can send), open the
# log file in 'a+' mode (as above) and trap 'HUP' to reopen the log file:
#
#   trap 'HUP' do log_file.reopen '/path/to/webrick.log', 'a+'
#
# == Copyright
#
# Author: IPR -- Internet Programming with Ruby -- writers
#
# Copyright (c) 2000 TAKAHASHI Masayoshi, GOTOU YUUZOU
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#--
# $IPR: webrick.rb,v 1.12 2002/10/01 17:16:31 gotoyuzo Exp $

module WEBrick
end

require 'webrick/compat.rb'

require 'webrick/version.rb'
require 'webrick/config.rb'
require 'webrick/log.rb'
require 'webrick/server.rb'
require 'webrick/utils.rb'
require 'webrick/accesslog'

require 'webrick/htmlutils.rb'
require 'webrick/httputils.rb'
require 'webrick/cookie.rb'
require 'webrick/httpversion.rb'
require 'webrick/httpstatus.rb'
require 'webrick/httprequest.rb'
require 'webrick/httpresponse.rb'
require 'webrick/httpserver.rb'
require 'webrick/httpservlet.rb'
require 'webrick/httpauth.rb'
