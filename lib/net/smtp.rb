# = net/smtp.rb
#
# Copyright (c) 1999-2007 Yukihiro Matsumoto.
#
# Copyright (c) 1999-2007 Minero Aoki.
#
# Written & maintained by Minero Aoki <aamine@loveruby.net>.
#
# Documented by William Webber and Minero Aoki.
#
# This program is free software. You can re-distribute and/or
# modify this program under the same terms as Ruby itself.
#
# NOTE: You can find Japanese version of this document at:
# http://www.ruby-lang.org/ja/man/html/net_smtp.html
#
# $Id$
#
# See Net::SMTP for documentation.
#

require 'net/protocol'
require 'digest/md5'
require 'timeout'
begin
  require 'openssl'
rescue LoadError
end

module Net

  # Module mixed in to all SMTP error classes
  module SMTPError
    # This *class* is a module for backward compatibility.
    # In later release, this module becomes a class.
  end

  # Represents an SMTP authentication error.
  class SMTPAuthenticationError < ProtoAuthError
    include SMTPError
  end

  # Represents SMTP error code 420 or 450, a temporary error.
  class SMTPServerBusy < ProtoServerError
    include SMTPError
  end

  # Represents an SMTP command syntax error (error code 500)
  class SMTPSyntaxError < ProtoSyntaxError
    include SMTPError
  end

  # Represents a fatal SMTP error (error code 5xx, except for 500)
  class SMTPFatalError < ProtoFatalError
    include SMTPError
  end

  # Unexpected reply code returned from server.
  class SMTPUnknownError < ProtoUnknownError
    include SMTPError
  end

  # Command is not supported on server.
  class SMTPUnsupportedCommand < ProtocolError
    include SMTPError
  end

  #
  # = Net::SMTP
  #
  # == What is This Library?
  #
  # This library provides functionality to send internet
  # mail via SMTP, the Simple Mail Transfer Protocol. For details of
  # SMTP itself, see [RFC2821] (http://www.ietf.org/rfc/rfc2821.txt).
  #
  # == What is This Library NOT?
  #
  # This library does NOT provide functions to compose internet mails.
  # You must create them by yourself. If you want better mail support,
  # try RubyMail or TMail. You can get both libraries from RAA.
  # (http://www.ruby-lang.org/en/raa.html)
  #
  # FYI: the official documentation on internet mail is: [RFC2822] (http://www.ietf.org/rfc/rfc2822.txt).
  #
  # == Examples
  #
  # === Sending Messages
  #
  # You must open a connection to an SMTP server before sending messages.
  # The first argument is the address of your SMTP server, and the second
  # argument is the port number. Using SMTP.start with a block is the simplest
  # way to do this. This way, the SMTP connection is closed automatically
  # after the block is executed.
  #
  #     require 'net/smtp'
  #     Net::SMTP.start('your.smtp.server', 25) do |smtp|
  #       # Use the SMTP object smtp only in this block.
  #     end
  #
  # Replace 'your.smtp.server' with your SMTP server. Normally
  # your system manager or internet provider supplies a server
  # for you.
  #
  # Then you can send messages.
  #
  #     msgstr = <<END_OF_MESSAGE
  #     From: Your Name <your@mail.address>
  #     To: Destination Address <someone@example.com>
  #     Subject: test message
  #     Date: Sat, 23 Jun 2001 16:26:43 +0900
  #     Message-Id: <unique.message.id.string@example.com>
  #
  #     This is a test message.
  #     END_OF_MESSAGE
  #
  #     require 'net/smtp'
  #     Net::SMTP.start('your.smtp.server', 25) do |smtp|
  #       smtp.send_message msgstr,
  #                         'your@mail.address',
  #                         'his_address@example.com'
  #     end
  #
  # === Closing the Session
  #
  # You MUST close the SMTP session after sending messages, by calling
  # the #finish method:
  #
  #     # using SMTP#finish
  #     smtp = Net::SMTP.start('your.smtp.server', 25)
  #     smtp.send_message msgstr, 'from@address', 'to@address'
  #     smtp.finish
  #
  # You can also use the block form of SMTP.start/SMTP#start.  This closes
  # the SMTP session automatically:
  #
  #     # using block form of SMTP.start
  #     Net::SMTP.start('your.smtp.server', 25) do |smtp|
  #       smtp.send_message msgstr, 'from@address', 'to@address'
  #     end
  #
  # I strongly recommend this scheme.  This form is simpler and more robust.
  #
  # === HELO domain
  #
  # In almost all situations, you must provide a third argument
  # to SMTP.start/SMTP#start. This is the domain name which you are on
  # (the host to send mail from). It is called the "HELO domain".
  # The SMTP server will judge whether it should send or reject
  # the SMTP session by inspecting the HELO domain.
  #
  #     Net::SMTP.start('your.smtp.server', 25,
  #                     'mail.from.domain') { |smtp| ... }
  #
  # === SMTP Authentication
  #
  # The Net::SMTP class supports three authentication schemes;
  # PLAIN, LOGIN and CRAM MD5.  (SMTP Authentication: [RFC2554])
  # To use SMTP authentication, pass extra arguments to
  # SMTP.start/SMTP#start.
  #
  #     # PLAIN
  #     Net::SMTP.start('your.smtp.server', 25, 'mail.from.domain',
  #                     'Your Account', 'Your Password', :plain)
  #     # LOGIN
  #     Net::SMTP.start('your.smtp.server', 25, 'mail.from.domain',
  #                     'Your Account', 'Your Password', :login)
  #
  #     # CRAM MD5
  #     Net::SMTP.start('your.smtp.server', 25, 'mail.from.domain',
  #                     'Your Account', 'Your Password', :cram_md5)
  #
  class SMTP

    Revision = %q$Revision$.split[1]

    # The default SMTP port number, 25.
    def SMTP.default_port
      25
    end

    # The default mail submission port number, 587.
    def SMTP.default_submission_port
      587
    end

    # The default SMTPS port number, 465.
    def SMTP.default_tls_port
      465
    end

    class << self
      alias default_ssl_port default_tls_port
    end

    def SMTP.default_ssl_context
      OpenSSL::SSL::SSLContext.new
    end

    #
    # Creates a new Net::SMTP object.
    #
    # +address+ is the hostname or ip address of your SMTP
    # server.  +port+ is the port to connect to; it defaults to
    # port 25.
    #
    # This method does not open the TCP connection.  You can use
    # SMTP.start instead of SMTP.new if you want to do everything
    # at once.  Otherwise, follow SMTP.new with SMTP#start.
    #
    def initialize(address, port = nil)
      @address = address
      @port = (port || SMTP.default_port)
      @esmtp = true
      @capabilities = nil
      @socket = nil
      @started = false
      @open_timeout = 30
      @read_timeout = 60
      @error_occured = false
      @debug_output = nil
      @tls = false
      @starttls = false
      @ssl_context = nil
    end

    # Provide human-readable stringification of class state.
    def inspect
      "#<#{self.class} #{@address}:#{@port} started=#{@started}>"
    end

    #
    # Set whether to use ESMTP or not.  This should be done before
    # calling #start.  Note that if #start is called in ESMTP mode,
    # and the connection fails due to a ProtocolError, the SMTP
    # object will automatically switch to plain SMTP mode and
    # retry (but not vice versa).
    #
    attr_accessor :esmtp

    # +true+ if the SMTP object uses ESMTP (which it does by default).
    alias :esmtp? :esmtp

    # true if server advertises STARTTLS.
    # You cannot get valid value before opening SMTP session.
    def capable_starttls?
      capable?('STARTTLS')
    end

    def capable?(key)
      return nil unless @capabilities
      @capabilities[key] ? true : false
    end
    private :capable?

    # true if server advertises AUTH PLAIN.
    # You cannot get valid value before opening SMTP session.
    def capable_plain_auth?
      auth_capable?('PLAIN')
    end

    # true if server advertises AUTH LOGIN.
    # You cannot get valid value before opening SMTP session.
    def capable_login_auth?
      auth_capable?('LOGIN')
    end

    # true if server advertises AUTH CRAM-MD5.
    # You cannot get valid value before opening SMTP session.
    def capable_cram_md5_auth?
      auth_capable?('CRAM-MD5')
    end

    def auth_capable?(type)
      return nil unless @capabilities
      return false unless @capabilities['AUTH']
      @capabilities['AUTH'].include?(type)
    end
    private :auth_capable?

    # Returns supported authentication methods on this server.
    # You cannot get valid value before opening SMTP session.
    def capable_auth_types
      return [] unless @capabilities
      return [] unless @capabilities['AUTH']
      @capabilities['AUTH']
    end

    # true if this object uses SMTP/TLS (SMTPS).
    def tls?
      @tls
    end

    alias ssl? tls?

    # Enables SMTP/TLS (SMTPS: SMTP over direct TLS connection) for
    # this object.  Must be called before the connection is established
    # to have any effect.  +context+ is a OpenSSL::SSL::SSLContext object.
    def enable_tls(context = SMTP.default_ssl_context)
      raise 'openssl library not installed' unless defined?(OpenSSL)
      raise ArgumentError, "SMTPS and STARTTLS is exclusive" if @starttls
      @tls = true
      @ssl_context = context
    end

    alias enable_ssl enable_tls

    # Disables SMTP/TLS for this object.  Must be called before the
    # connection is established to have any effect.
    def disable_tls
      @tls = false
      @ssl_context = nil
    end

    alias disable_ssl disable_tls

    # Returns truth value if this object uses STARTTLS.
    # If this object always uses STARTTLS, returns :always.
    # If this object uses STARTTLS when the server support TLS, returns :auto.
    def starttls?
      @starttls
    end

    # true if this object uses STARTTLS.
    def starttls_always?
      @starttls == :always
    end

    # true if this object uses STARTTLS when server advertises STARTTLS.
    def starttls_auto?
      @starttls == :auto
    end

    # Enables SMTP/TLS (STARTTLS) for this object.
    # +context+ is a OpenSSL::SSL::SSLContext object.
    def enable_starttls(context = SMTP.default_ssl_context)
      raise 'openssl library not installed' unless defined?(OpenSSL)
      raise ArgumentError, "SMTPS and STARTTLS is exclusive" if @tls
      @starttls = :always
      @ssl_context = context
    end

    # Enables SMTP/TLS (STARTTLS) for this object if server accepts.
    # +context+ is a OpenSSL::SSL::SSLContext object.
    def enable_starttls_auto(context = SMTP.default_ssl_context)
      raise 'openssl library not installed' unless defined?(OpenSSL)
      raise ArgumentError, "SMTPS and STARTTLS is exclusive" if @tls
      @starttls = :auto
      @ssl_context = context
    end

    # Disables SMTP/TLS (STARTTLS) for this object.  Must be called
    # before the connection is established to have any effect.
    def disable_starttls
      @starttls = false
      @ssl_context = nil
    end

    # The address of the SMTP server to connect to.
    attr_reader :address

    # The port number of the SMTP server to connect to.
    attr_reader :port

    # Seconds to wait while attempting to open a connection.
    # If the connection cannot be opened within this time, a
    # TimeoutError is raised.
    attr_accessor :open_timeout

    # Seconds to wait while reading one block (by one read(2) call).
    # If the read(2) call does not complete within this time, a
    # TimeoutError is raised.
    attr_reader :read_timeout

    # Set the number of seconds to wait until timing-out a read(2)
    # call.
    def read_timeout=(sec)
      @socket.read_timeout = sec if @socket
      @read_timeout = sec
    end

    #
    # WARNING: This method causes serious security holes.
    # Use this method for only debugging.
    #
    # Set an output stream for debug logging.
    # You must call this before #start.
    #
    #   # example
    #   smtp = Net::SMTP.new(addr, port)
    #   smtp.set_debug_output $stderr
    #   smtp.start do |smtp|
    #     ....
    #   end
    #
    def debug_output=(arg)
      @debug_output = arg
    end

    alias set_debug_output debug_output=

    #
    # SMTP session control
    #

    #
    # Creates a new Net::SMTP object and connects to the server.
    #
    # This method is equivalent to:
    #
    #   Net::SMTP.new(address, port).start(helo_domain, account, password, authtype)
    #
    # === Example
    #
    #     Net::SMTP.start('your.smtp.server') do |smtp|
    #       smtp.send_message msgstr, 'from@example.com', ['dest@example.com']
    #     end
    #
    # === Block Usage
    #
    # If called with a block, the newly-opened Net::SMTP object is yielded
    # to the block, and automatically closed when the block finishes.  If called
    # without a block, the newly-opened Net::SMTP object is returned to
    # the caller, and it is the caller's responsibility to close it when
    # finished.
    #
    # === Parameters
    #
    # +address+ is the hostname or ip address of your smtp server.
    #
    # +port+ is the port to connect to; it defaults to port 25.
    #
    # +helo+ is the _HELO_ _domain_ provided by the client to the
    # server (see overview comments); it defaults to 'localhost'.
    #
    # The remaining arguments are used for SMTP authentication, if required
    # or desired.  +user+ is the account name; +secret+ is your password
    # or other authentication token; and +authtype+ is the authentication
    # type, one of :plain, :login, or :cram_md5.  See the discussion of
    # SMTP Authentication in the overview notes.
    #
    # === Errors
    #
    # This method may raise:
    #
    # * Net::SMTPAuthenticationError
    # * Net::SMTPServerBusy
    # * Net::SMTPSyntaxError
    # * Net::SMTPFatalError
    # * Net::SMTPUnknownError
    # * IOError
    # * TimeoutError
    #
    def SMTP.start(address, port = nil, helo = 'localhost',
                   user = nil, secret = nil, authtype = nil,
                   &block)   # :yield: smtp
      new(address, port).start(helo, user, secret, authtype, &block)
    end

    # +true+ if the SMTP session has been started.
    def started?
      @started
    end

    #
    # Opens a TCP connection and starts the SMTP session.
    #
    # === Parameters
    #
    # +helo+ is the _HELO_ _domain_ that you'll dispatch mails from; see
    # the discussion in the overview notes.
    #
    # If both of +user+ and +secret+ are given, SMTP authentication
    # will be attempted using the AUTH command.  +authtype+ specifies
    # the type of authentication to attempt; it must be one of
    # :login, :plain, and :cram_md5.  See the notes on SMTP Authentication
    # in the overview.
    #
    # === Block Usage
    #
    # When this methods is called with a block, the newly-started SMTP
    # object is yielded to the block, and automatically closed after
    # the block call finishes.  Otherwise, it is the caller's
    # responsibility to close the session when finished.
    #
    # === Example
    #
    # This is very similar to the class method SMTP.start.
    #
    #     require 'net/smtp'
    #     smtp = Net::SMTP.new('smtp.mail.server', 25)
    #     smtp.start(helo_domain, account, password, authtype) do |smtp|
    #       smtp.send_message msgstr, 'from@example.com', ['dest@example.com']
    #     end
    #
    # The primary use of this method (as opposed to SMTP.start)
    # is probably to set debugging (#set_debug_output) or ESMTP
    # (#esmtp=), which must be done before the session is
    # started.
    #
    # === Errors
    #
    # If session has already been started, an IOError will be raised.
    #
    # This method may raise:
    #
    # * Net::SMTPAuthenticationError
    # * Net::SMTPServerBusy
    # * Net::SMTPSyntaxError
    # * Net::SMTPFatalError
    # * Net::SMTPUnknownError
    # * IOError
    # * TimeoutError
    #
    def start(helo = 'localhost',
              user = nil, secret = nil, authtype = nil)   # :yield: smtp
      if block_given?
        begin
          do_start helo, user, secret, authtype
          return yield(self)
        ensure
          do_finish
        end
      else
        do_start helo, user, secret, authtype
        return self
      end
    end

    # Finishes the SMTP session and closes TCP connection.
    # Raises IOError if not started.
    def finish
      raise IOError, 'not yet started' unless started?
      do_finish
    end

    private

    def tcp_socket(address, port)
      TCPSocket.open address, port
    end

    def do_start(helo_domain, user, secret, authtype)
      raise IOError, 'SMTP session already started' if @started
      if user or secret
        check_auth_method(authtype || DEFAULT_AUTH_TYPE)
        check_auth_args user, secret
      end
      s = Timeout.timeout(@open_timeout, Net::OpenTimeout) do
        tcp_socket(@address, @port)
      end
      logging "Connection opened: #{@address}:#{@port}"
      @socket = new_internet_message_io(tls? ? tlsconnect(s) : s)
      check_response critical { recv_response() }
      do_helo helo_domain
      if starttls_always? or (capable_starttls? and starttls_auto?)
        unless capable_starttls?
          raise SMTPUnsupportedCommand,
              "STARTTLS is not supported on this server"
        end
        starttls
        @socket = new_internet_message_io(tlsconnect(s))
        # helo response may be different after STARTTLS
        do_helo helo_domain
      end
      authenticate user, secret, (authtype || DEFAULT_AUTH_TYPE) if user
      @started = true
    ensure
      unless @started
        # authentication failed, cancel connection.
        s.close if s and not s.closed?
        @socket = nil
      end
    end

    def ssl_socket(socket, context)
      OpenSSL::SSL::SSLSocket.new socket, context
    end

    def tlsconnect(s)
      verified = false
      s = ssl_socket(s, @ssl_context)
      logging "TLS connection started"
      s.sync_close = true
      s.connect
      if @ssl_context.verify_mode != OpenSSL::SSL::VERIFY_NONE
        s.post_connection_check(@address)
      end
      verified = true
      s
    ensure
      s.close unless verified
    end

    def new_internet_message_io(s)
      io = InternetMessageIO.new(s)
      io.read_timeout = @read_timeout
      io.debug_output = @debug_output
      io
    end

    def do_helo(helo_domain)
      res = @esmtp ? ehlo(helo_domain) : helo(helo_domain)
      @capabilities = res.capabilities
    rescue SMTPError
      if @esmtp
        @esmtp = false
        @error_occured = false
        retry
      end
      raise
    end

    def do_finish
      quit if @socket and not @socket.closed? and not @error_occured
    ensure
      @started = false
      @error_occured = false
      @socket.close if @socket and not @socket.closed?
      @socket = nil
    end

    #
    # Message Sending
    #

    public

    #
    # Sends +msgstr+ as a message.  Single CR ("\r") and LF ("\n") found
    # in the +msgstr+, are converted into the CR LF pair.  You cannot send a
    # binary message with this method. +msgstr+ should include both
    # the message headers and body.
    #
    # +from_addr+ is a String representing the source mail address.
    #
    # +to_addr+ is a String or Strings or Array of Strings, representing
    # the destination mail address or addresses.
    #
    # === Example
    #
    #     Net::SMTP.start('smtp.example.com') do |smtp|
    #       smtp.send_message msgstr,
    #                         'from@example.com',
    #                         ['dest@example.com', 'dest2@example.com']
    #     end
    #
    # === Errors
    #
    # This method may raise:
    #
    # * Net::SMTPServerBusy
    # * Net::SMTPSyntaxError
    # * Net::SMTPFatalError
    # * Net::SMTPUnknownError
    # * IOError
    # * TimeoutError
    #
    def send_message(msgstr, from_addr, *to_addrs)
      raise IOError, 'closed session' unless @socket
      mailfrom from_addr
      rcptto_list(to_addrs) {data msgstr}
    end

    alias send_mail send_message
    alias sendmail send_message   # obsolete

    #
    # Opens a message writer stream and gives it to the block.
    # The stream is valid only in the block, and has these methods:
    #
    # puts(str = '')::       outputs STR and CR LF.
    # print(str)::           outputs STR.
    # printf(fmt, *args)::   outputs sprintf(fmt,*args).
    # write(str)::           outputs STR and returns the length of written bytes.
    # <<(str)::              outputs STR and returns self.
    #
    # If a single CR ("\r") or LF ("\n") is found in the message,
    # it is converted to the CR LF pair.  You cannot send a binary
    # message with this method.
    #
    # === Parameters
    #
    # +from_addr+ is a String representing the source mail address.
    #
    # +to_addr+ is a String or Strings or Array of Strings, representing
    # the destination mail address or addresses.
    #
    # === Example
    #
    #     Net::SMTP.start('smtp.example.com', 25) do |smtp|
    #       smtp.open_message_stream('from@example.com', ['dest@example.com']) do |f|
    #         f.puts 'From: from@example.com'
    #         f.puts 'To: dest@example.com'
    #         f.puts 'Subject: test message'
    #         f.puts
    #         f.puts 'This is a test message.'
    #       end
    #     end
    #
    # === Errors
    #
    # This method may raise:
    #
    # * Net::SMTPServerBusy
    # * Net::SMTPSyntaxError
    # * Net::SMTPFatalError
    # * Net::SMTPUnknownError
    # * IOError
    # * TimeoutError
    #
    def open_message_stream(from_addr, *to_addrs, &block)   # :yield: stream
      raise IOError, 'closed session' unless @socket
      mailfrom from_addr
      rcptto_list(to_addrs) {data(&block)}
    end

    alias ready open_message_stream   # obsolete

    #
    # Authentication
    #

    public

    DEFAULT_AUTH_TYPE = :plain

    def authenticate(user, secret, authtype = DEFAULT_AUTH_TYPE)
      check_auth_method authtype
      check_auth_args user, secret
      send auth_method(authtype), user, secret
    end

    def auth_plain(user, secret)
      check_auth_args user, secret
      res = critical {
        get_response('AUTH PLAIN ' + base64_encode("\0#{user}\0#{secret}"))
      }
      check_auth_response res
      res
    end

    def auth_login(user, secret)
      check_auth_args user, secret
      res = critical {
        check_auth_continue get_response('AUTH LOGIN')
        check_auth_continue get_response(base64_encode(user))
        get_response(base64_encode(secret))
      }
      check_auth_response res
      res
    end

    def auth_cram_md5(user, secret)
      check_auth_args user, secret
      res = critical {
        res0 = get_response('AUTH CRAM-MD5')
        check_auth_continue res0
        crammed = cram_md5_response(secret, res0.cram_md5_challenge)
        get_response(base64_encode("#{user} #{crammed}"))
      }
      check_auth_response res
      res
    end

    private

    def check_auth_method(type)
      unless respond_to?(auth_method(type), true)
        raise ArgumentError, "wrong authentication type #{type}"
      end
    end

    def auth_method(type)
      "auth_#{type.to_s.downcase}".intern
    end

    def check_auth_args(user, secret, authtype = DEFAULT_AUTH_TYPE)
      unless user
        raise ArgumentError, 'SMTP-AUTH requested but missing user name'
      end
      unless secret
        raise ArgumentError, 'SMTP-AUTH requested but missing secret phrase'
      end
    end

    def base64_encode(str)
      # expects "str" may not become too long
      [str].pack('m').gsub(/\s+/, '')
    end

    IMASK = 0x36
    OMASK = 0x5c

    # CRAM-MD5: [RFC2195]
    def cram_md5_response(secret, challenge)
      tmp = Digest::MD5.digest(cram_secret(secret, IMASK) + challenge)
      Digest::MD5.hexdigest(cram_secret(secret, OMASK) + tmp)
    end

    CRAM_BUFSIZE = 64

    def cram_secret(secret, mask)
      secret = Digest::MD5.digest(secret) if secret.size > CRAM_BUFSIZE
      buf = secret.ljust(CRAM_BUFSIZE, "\0")
      0.upto(buf.size - 1) do |i|
        buf[i] = (buf[i].ord ^ mask).chr
      end
      buf
    end

    #
    # SMTP command dispatcher
    #

    public

    def starttls
      getok('STARTTLS')
    end

    def helo(domain)
      getok("HELO #{domain}")
    end

    def ehlo(domain)
      getok("EHLO #{domain}")
    end

    def mailfrom(from_addr)
      if $SAFE > 0
        raise SecurityError, 'tainted from_addr' if from_addr.tainted?
      end
      getok("MAIL FROM:<#{from_addr}>")
    end

    def rcptto_list(to_addrs)
      raise ArgumentError, 'mail destination not given' if to_addrs.empty?
      ok_users = []
      unknown_users = []
      to_addrs.flatten.each do |addr|
        begin
          rcptto addr
        rescue SMTPAuthenticationError
          unknown_users << addr.dump
        else
          ok_users << addr
        end
      end
      raise ArgumentError, 'mail destination not given' if ok_users.empty?
      ret = yield
      unless unknown_users.empty?
        raise SMTPAuthenticationError, "failed to deliver for #{unknown_users.join(', ')}"
      end
      ret
    end

    def rcptto(to_addr)
      if $SAFE > 0
        raise SecurityError, 'tainted to_addr' if to_addr.tainted?
      end
      getok("RCPT TO:<#{to_addr}>")
    end

    # This method sends a message.
    # If +msgstr+ is given, sends it as a message.
    # If block is given, yield a message writer stream.
    # You must write message before the block is closed.
    #
    #   # Example 1 (by string)
    #   smtp.data(<<EndMessage)
    #   From: john@example.com
    #   To: betty@example.com
    #   Subject: I found a bug
    #
    #   Check vm.c:58879.
    #   EndMessage
    #
    #   # Example 2 (by block)
    #   smtp.data {|f|
    #     f.puts "From: john@example.com"
    #     f.puts "To: betty@example.com"
    #     f.puts "Subject: I found a bug"
    #     f.puts ""
    #     f.puts "Check vm.c:58879."
    #   }
    #
    def data(msgstr = nil, &block)   #:yield: stream
      if msgstr and block
        raise ArgumentError, "message and block are exclusive"
      end
      unless msgstr or block
        raise ArgumentError, "message or block is required"
      end
      res = critical {
        check_continue get_response('DATA')
        if msgstr
          @socket.write_message msgstr
        else
          @socket.write_message_by_block(&block)
        end
        recv_response()
      }
      check_response res
      res
    end

    def quit
      getok('QUIT')
    end

    private

    def getok(reqline)
      res = critical {
        @socket.writeline reqline
        recv_response()
      }
      check_response res
      res
    end

    def get_response(reqline)
      @socket.writeline reqline
      recv_response()
    end

    def recv_response
      buf = ''
      while true
        line = @socket.readline
        buf << line << "\n"
        break unless line[3,1] == '-'   # "210-PIPELINING"
      end
      Response.parse(buf)
    end

    def critical
      return '200 dummy reply code' if @error_occured
      begin
        return yield()
      rescue Exception
        @error_occured = true
        raise
      end
    end

    def check_response(res)
      unless res.success?
        raise res.exception_class, res.message
      end
    end

    def check_continue(res)
      unless res.continue?
        raise SMTPUnknownError, "could not get 3xx (#{res.status})"
      end
    end

    def check_auth_response(res)
      unless res.success?
        raise SMTPAuthenticationError, res.message
      end
    end

    def check_auth_continue(res)
      unless res.continue?
        raise res.exception_class, res.message
      end
    end

    # This class represents a response received by the SMTP server. Instances
    # of this class are created by the SMTP class; they should not be directly
    # created by the user. For more information on SMTP responses, view
    # {Section 4.2 of RFC 5321}[http://tools.ietf.org/html/rfc5321#section-4.2]
    class Response
      # Parses the received response and separates the reply code and the human
      # readable reply text
      def self.parse(str)
        new(str[0,3], str)
      end

      # Creates a new instance of the Response class and sets the status and
      # string attributes
      def initialize(status, string)
        @status = status
        @string = string
      end

      # The three digit reply code of the SMTP response
      attr_reader :status

      # The human readable reply text of the SMTP response
      attr_reader :string

      # Takes the first digit of the reply code to determine the status type
      def status_type_char
        @status[0, 1]
      end

      # Determines whether the response received was a Positive Completion
      # reply (2xx reply code)
      def success?
        status_type_char() == '2'
      end

      # Determines whether the response received was a Positive Intermediate
      # reply (3xx reply code)
      def continue?
        status_type_char() == '3'
      end

      # The first line of the human readable reply text
      def message
        @string.lines.first
      end

      # Creates a CRAM-MD5 challenge. You can view more information on CRAM-MD5
      # on Wikipedia: http://en.wikipedia.org/wiki/CRAM-MD5
      def cram_md5_challenge
        @string.split(/ /)[1].unpack('m')[0]
      end

      # Returns a hash of the human readable reply text in the response if it
      # is multiple lines. It does not return the first line. The key of the
      # hash is the first word the value of the hash is an array with each word
      # thereafter being a value in the array
      def capabilities
        return {} unless @string[3, 1] == '-'
        h = {}
        @string.lines.drop(1).each do |line|
          k, *v = line[4..-1].chomp.split
          h[k] = v
        end
        h
      end

      # Determines whether there was an error and raies the appropriate error
      # based on the reply code of the response
      def exception_class
        case @status
        when /\A4/  then SMTPServerBusy
        when /\A50/ then SMTPSyntaxError
        when /\A53/ then SMTPAuthenticationError
        when /\A5/  then SMTPFatalError
        else             SMTPUnknownError
        end
      end
    end

    def logging(msg)
      @debug_output << msg + "\n" if @debug_output
    end

  end   # class SMTP

  SMTPSession = SMTP

end
