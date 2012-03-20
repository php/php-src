#
# server.rb -- GenericServer Class
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000, 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: server.rb,v 1.62 2003/07/22 19:20:43 gotoyuzo Exp $

require 'thread'
require 'socket'
require 'webrick/config'
require 'webrick/log'

module WEBrick

  class ServerError < StandardError; end

  class SimpleServer
    def SimpleServer.start
      yield
    end
  end

  ##
  # A generic module for daemonizing a process

  class Daemon

    ##
    # Performs the standard operations for daemonizing a process.  Runs a
    # block, if given.

    def Daemon.start
      exit!(0) if fork
      Process::setsid
      exit!(0) if fork
      Dir::chdir("/")
      File::umask(0)
      STDIN.reopen("/dev/null")
      STDOUT.reopen("/dev/null", "w")
      STDERR.reopen("/dev/null", "w")
      yield if block_given?
    end
  end

  class GenericServer
    attr_reader :status, :config, :logger, :tokens, :listeners

    def initialize(config={}, default=Config::General)
      @config = default.dup.update(config)
      @status = :Stop
      @config[:Logger] ||= Log::new
      @logger = @config[:Logger]

      @tokens = SizedQueue.new(@config[:MaxClients])
      @config[:MaxClients].times{ @tokens.push(nil) }

      webrickv = WEBrick::VERSION
      rubyv = "#{RUBY_VERSION} (#{RUBY_RELEASE_DATE}) [#{RUBY_PLATFORM}]"
      @logger.info("WEBrick #{webrickv}")
      @logger.info("ruby #{rubyv}")

      @listeners = []
      unless @config[:DoNotListen]
        if @config[:Listen]
          warn(":Listen option is deprecated; use GenericServer#listen")
        end
        listen(@config[:BindAddress], @config[:Port])
        if @config[:Port] == 0
          @config[:Port] = @listeners[0].addr[1]
        end
      end
    end

    def [](key)
      @config[key]
    end

    def listen(address, port)
      @listeners += Utils::create_listeners(address, port, @logger)
    end

    def start(&block)
      raise ServerError, "already started." if @status != :Stop
      server_type = @config[:ServerType] || SimpleServer

      server_type.start{
        @logger.info \
          "#{self.class}#start: pid=#{$$} port=#{@config[:Port]}"
        call_callback(:StartCallback)

        thgroup = ThreadGroup.new
        @status = :Running
        while @status == :Running
          begin
            if svrs = IO.select(@listeners, nil, nil, 2.0)
              svrs[0].each{|svr|
                @tokens.pop          # blocks while no token is there.
                if sock = accept_client(svr)
                  sock.do_not_reverse_lookup = config[:DoNotReverseLookup]
                  th = start_thread(sock, &block)
                  th[:WEBrickThread] = true
                  thgroup.add(th)
                else
                  @tokens.push(nil)
                end
              }
            end
          rescue Errno::EBADF, IOError => ex
            # if the listening socket was closed in GenericServer#shutdown,
            # IO::select raise it.
          rescue Exception => ex
            msg = "#{ex.class}: #{ex.message}\n\t#{ex.backtrace[0]}"
            @logger.error msg
          end
        end

        @logger.info "going to shutdown ..."
        thgroup.list.each{|th| th.join if th[:WEBrickThread] }
        call_callback(:StopCallback)
        @logger.info "#{self.class}#start done."
        @status = :Stop
      }
    end

    def stop
      if @status == :Running
        @status = :Shutdown
      end
    end

    def shutdown
      stop
      @listeners.each{|s|
        if @logger.debug?
          addr = s.addr
          @logger.debug("close TCPSocket(#{addr[2]}, #{addr[1]})")
        end
        begin
          s.shutdown
        rescue Errno::ENOTCONN
          # when `Errno::ENOTCONN: Socket is not connected' on some platforms,
          # call #close instead of #shutdown.
          # (ignore @config[:ShutdownSocketWithoutClose])
          s.close
        else
          unless @config[:ShutdownSocketWithoutClose]
            s.close
          end
        end
      }
      @listeners.clear
    end

    def run(sock)
      @logger.fatal "run() must be provided by user."
    end

    private

    def accept_client(svr)
      sock = nil
      begin
        sock = svr.accept
        sock.sync = true
        Utils::set_non_blocking(sock)
        Utils::set_close_on_exec(sock)
      rescue Errno::ECONNRESET, Errno::ECONNABORTED,
             Errno::EPROTO, Errno::EINVAL => ex
      rescue Exception => ex
        msg = "#{ex.class}: #{ex.message}\n\t#{ex.backtrace[0]}"
        @logger.error msg
      end
      return sock
    end

    def start_thread(sock, &block)
      Thread.start{
        begin
          Thread.current[:WEBrickSocket] = sock
          begin
            addr = sock.peeraddr
            @logger.debug "accept: #{addr[3]}:#{addr[1]}"
          rescue SocketError
            @logger.debug "accept: <address unknown>"
            raise
          end
          call_callback(:AcceptCallback, sock)
          block ? block.call(sock) : run(sock)
        rescue Errno::ENOTCONN
          @logger.debug "Errno::ENOTCONN raised"
        rescue ServerError => ex
          msg = "#{ex.class}: #{ex.message}\n\t#{ex.backtrace[0]}"
          @logger.error msg
        rescue Exception => ex
          @logger.error ex
        ensure
          @tokens.push(nil)
          Thread.current[:WEBrickSocket] = nil
          if addr
            @logger.debug "close: #{addr[3]}:#{addr[1]}"
          else
            @logger.debug "close: <address unknown>"
          end
          sock.close
        end
      }
    end

    def call_callback(callback_name, *args)
      if cb = @config[callback_name]
        cb.call(*args)
      end
    end
  end    # end of GenericServer
end
