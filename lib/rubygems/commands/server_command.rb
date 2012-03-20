require 'rubygems/command'
require 'rubygems/server'

class Gem::Commands::ServerCommand < Gem::Command

  def initialize
    super 'server', 'Documentation and gem repository HTTP server',
          :port => 8808, :gemdir => [], :daemon => false

    OptionParser.accept :Port do |port|
      if port =~ /\A\d+\z/ then
        port = Integer port
        raise OptionParser::InvalidArgument, "#{port}: not a port number" if
          port > 65535

        port
      else
        begin
          Socket.getservbyname port
        rescue SocketError
          raise OptionParser::InvalidArgument, "#{port}: no such named service"
        end
      end
    end

    add_option '-p', '--port=PORT', :Port,
               'port to listen on' do |port, options|
      options[:port] = port
    end

    add_option '-d', '--dir=GEMDIR',
               'directories from which to serve gems',
               'multiple directories may be provided' do |gemdir, options|
      options[:gemdir] << File.expand_path(gemdir)
    end

    add_option '--[no-]daemon', 'run as a daemon' do |daemon, options|
      options[:daemon] = daemon
    end

    add_option '-b', '--bind=HOST,HOST',
               'addresses to bind', Array do |address, options|
      options[:addresses] ||= []
      options[:addresses].push(*address)
    end

    add_option '-l', '--launch[=COMMAND]',
               'launches a browser window',
               "COMMAND defaults to 'start' on Windows",
               "and 'open' on all other platforms" do |launch, options|
      launch ||= Gem.win_platform? ? 'start' : 'open'
      options[:launch] = launch
    end
  end

  def defaults_str # :nodoc:
    "--port 8808 --dir #{Gem.dir} --no-daemon"
  end

  def description # :nodoc:
    <<-EOF
The server command starts up a web server that hosts the RDoc for your
installed gems and can operate as a server for installation of gems on other
machines.

The cache files for installed gems must exist to use the server as a source
for gem installation.

To install gems from a running server, use `gem install GEMNAME --source
http://gem_server_host:8808`

You can set up a shortcut to gem server documentation using the URL:

  http://localhost:8808/rdoc?q=%s - Firefox
  http://localhost:8808/rdoc?q=* - LaunchBar

    EOF
  end

  def execute
    options[:gemdir] << Gem.dir if options[:gemdir].empty?
    Gem::Server.run options
  end

end

