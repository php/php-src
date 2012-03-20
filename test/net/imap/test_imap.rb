require "net/imap"
require "test/unit"

class IMAPTest < Test::Unit::TestCase
  CA_FILE = File.expand_path("cacert.pem", File.dirname(__FILE__))
  SERVER_KEY = File.expand_path("server.key", File.dirname(__FILE__))
  SERVER_CERT = File.expand_path("server.crt", File.dirname(__FILE__))

  SERVER_ADDR = "127.0.0.1"

  def setup
    @do_not_reverse_lookup = Socket.do_not_reverse_lookup
    Socket.do_not_reverse_lookup = true
  end

  def teardown
    Socket.do_not_reverse_lookup = @do_not_reverse_lookup
  end

  def test_encode_utf7
    utf8 = "\357\274\241\357\274\242\357\274\243".force_encoding("UTF-8")
    s = Net::IMAP.encode_utf7(utf8)
    assert_equal("&,yH,Iv8j-".force_encoding("UTF-8"), s)

    utf8 = "\343\201\202&".force_encoding("UTF-8")
    s = Net::IMAP.encode_utf7(utf8)
    assert_equal("&MEI-&-".force_encoding("UTF-8"), s)
  end

  def test_decode_utf7
    s = Net::IMAP.decode_utf7("&,yH,Iv8j-")
    utf8 = "\357\274\241\357\274\242\357\274\243".force_encoding("UTF-8")
    assert_equal(utf8, s)
  end

  def test_format_date
    time = Time.mktime(2009, 7, 24)
    s = Net::IMAP.format_date(time)
    assert_equal("24-Jul-2009", s)
  end

  def test_format_datetime
    time = Time.mktime(2009, 7, 24, 1, 23, 45)
    s = Net::IMAP.format_datetime(time)
    assert_match(/\A24-Jul-2009 01:23 [+\-]\d{4}\z/, s)
  end

  def test_imaps_unknown_ca
    if defined?(OpenSSL)
      assert_raise(OpenSSL::SSL::SSLError) do
        imaps_test do |port|
          begin
            Net::IMAP.new("localhost",
                          :port => port,
                          :ssl => true)
          rescue SystemCallError
            skip $!
          end
        end
      end
    end
  end

  def test_imaps_with_ca_file
    if defined?(OpenSSL)
      assert_nothing_raised do
        imaps_test do |port|
          begin
            Net::IMAP.new("localhost",
                          :port => port,
                          :ssl => { :ca_file => CA_FILE })
          rescue SystemCallError
            skip $!
          end
        end
      end
    end
  end

  def test_imaps_verify_none
    if defined?(OpenSSL)
      assert_nothing_raised do
        imaps_test do |port|
          Net::IMAP.new(SERVER_ADDR,
                        :port => port,
                        :ssl => { :verify_mode => OpenSSL::SSL::VERIFY_NONE })
        end
      end
    end
  end

  def test_imaps_post_connection_check
    if defined?(OpenSSL)
      assert_raise(OpenSSL::SSL::SSLError) do
        imaps_test do |port|
          # SERVER_ADDR is different from the hostname in the certificate,
          # so the following code should raise a SSLError.
          Net::IMAP.new(SERVER_ADDR,
                        :port => port,
                        :ssl => { :ca_file => CA_FILE })
        end
      end
    end
  end

  def test_starttls
    imap = nil
    if defined?(OpenSSL)
      starttls_test do |port|
        imap = Net::IMAP.new("localhost", :port => port)
        imap.starttls(:ca_file => CA_FILE)
        imap
      end
    end
  rescue SystemCallError
    skip $!
  ensure
    if imap && !imap.disconnected?
      imap.disconnect
    end
  end

  def test_unexpected_eof
    server = create_tcp_server
    port = server.addr[1]
    Thread.start do
      begin
        sock = server.accept
        begin
          sock.print("* OK test server\r\n")
          sock.gets
#          sock.print("* BYE terminating connection\r\n")
#          sock.print("RUBY0001 OK LOGOUT completed\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = Net::IMAP.new(SERVER_ADDR, :port => port)
        assert_raise(EOFError) do
          imap.logout
        end
      ensure
        imap.disconnect if imap
      end
    ensure
      server.close
    end
  end

  def test_idle
    server = create_tcp_server
    port = server.addr[1]
    requests = []
    Thread.start do
      begin
        sock = server.accept
        begin
          sock.print("* OK test server\r\n")
          requests.push(sock.gets)
          sock.print("+ idling\r\n")
          sock.print("* 3 EXISTS\r\n")
          sock.print("* 2 EXPUNGE\r\n")
          requests.push(sock.gets)
          sock.print("RUBY0001 OK IDLE terminated\r\n")
          sock.gets
          sock.print("* BYE terminating connection\r\n")
          sock.print("RUBY0002 OK LOGOUT completed\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = Net::IMAP.new(SERVER_ADDR, :port => port)
        responses = []
        imap.idle do |res|
          responses.push(res)
          if res.name == "EXPUNGE"
            imap.idle_done
          end
        end
        assert_equal(3, responses.length)
        assert_instance_of(Net::IMAP::ContinuationRequest, responses[0])
        assert_equal("EXISTS", responses[1].name)
        assert_equal(3, responses[1].data)
        assert_equal("EXPUNGE", responses[2].name)
        assert_equal(2, responses[2].data)
        assert_equal(2, requests.length)
        assert_equal("RUBY0001 IDLE\r\n", requests[0])
        assert_equal("DONE\r\n", requests[1])
        imap.logout
      ensure
        imap.disconnect if imap
      end
    ensure
      server.close
    end
  end

  def test_exception_during_idle
    server = create_tcp_server
    port = server.addr[1]
    requests = []
    Thread.start do
      begin
        sock = server.accept
        begin
          sock.print("* OK test server\r\n")
          requests.push(sock.gets)
          sock.print("+ idling\r\n")
          sock.print("* 3 EXISTS\r\n")
          sock.print("* 2 EXPUNGE\r\n")
          requests.push(sock.gets)
          sock.print("RUBY0001 OK IDLE terminated\r\n")
          sock.gets
          sock.print("* BYE terminating connection\r\n")
          sock.print("RUBY0002 OK LOGOUT completed\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = Net::IMAP.new(SERVER_ADDR, :port => port)
        begin
          th = Thread.current
          m = Monitor.new
          in_idle = false
          exception_raised = false
          c = m.new_cond
          Thread.start do
            m.synchronize do
              until in_idle
                c.wait(0.1)
              end
            end
            th.raise(Interrupt)
            exception_raised = true
          end
          imap.idle do |res|
            m.synchronize do
              in_idle = true
              c.signal
              until exception_raised
                c.wait(0.1)
              end
            end
          end
        rescue Interrupt
        end
        assert_equal(2, requests.length)
        assert_equal("RUBY0001 IDLE\r\n", requests[0])
        assert_equal("DONE\r\n", requests[1])
        imap.logout
      ensure
        imap.disconnect if imap
      end
    ensure
      server.close
    end
  end

  def test_idle_done_not_during_idle
    server = create_tcp_server
    port = server.addr[1]
    requests = []
    Thread.start do
      begin
        sock = server.accept
        begin
          sock.print("* OK test server\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = Net::IMAP.new(SERVER_ADDR, :port => port)
        assert_raise(Net::IMAP::Error) do
          imap.idle_done
        end
      ensure
        imap.disconnect if imap
      end
    ensure
      server.close
    end
  end

  def test_unexpected_bye
    server = create_tcp_server
    port = server.addr[1]
    Thread.start do
      begin
        sock = server.accept
        begin
          sock.print("* OK Gimap ready for requests from 75.101.246.151 33if2752585qyk.26\r\n")
          sock.gets
          sock.print("* BYE System Error 33if2752585qyk.26\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = Net::IMAP.new(SERVER_ADDR, :port => port)
        assert_raise(Net::IMAP::ByeResponseError) do
          imap.login("user", "password")
        end
      end
    ensure
      server.close
    end
  end

  def test_exception_during_shutdown
    server = create_tcp_server
    port = server.addr[1]
    Thread.start do
      begin
        sock = server.accept
        begin
          sock.print("* OK test server\r\n")
          sock.gets
          sock.print("* BYE terminating connection\r\n")
          sock.print("RUBY0001 OK LOGOUT completed\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = Net::IMAP.new(SERVER_ADDR, :port => port)
        imap.instance_eval do
          def @sock.shutdown(*args)
            super
          ensure
            raise "error"
          end
        end
        imap.logout
      ensure
        assert_raise(RuntimeError) do
          imap.disconnect
        end
      end
    ensure
      server.close
    end
  end

  def test_connection_closed_during_idle
    server = create_tcp_server
    port = server.addr[1]
    requests = []
    sock = nil
    Thread.start do
      begin
        sock = server.accept
        sock.print("* OK test server\r\n")
        requests.push(sock.gets)
        sock.print("+ idling\r\n")
      rescue
      end
    end
    begin
      imap = Net::IMAP.new(SERVER_ADDR, :port => port)
      begin
        th = Thread.current
        m = Monitor.new
        in_idle = false
        exception_raised = false
        c = m.new_cond
        Thread.start do
          m.synchronize do
            until in_idle
              c.wait(0.1)
            end
          end
          sock.close
          exception_raised = true
        end
        assert_raise(Net::IMAP::Error) do
          imap.idle do |res|
            m.synchronize do
              in_idle = true
              c.signal
              until exception_raised
                c.wait(0.1)
              end
            end
          end
        end
        assert_equal(1, requests.length)
        assert_equal("RUBY0001 IDLE\r\n", requests[0])
      ensure
        imap.disconnect if imap
      end
    ensure
      server.close
      if sock && !sock.closed?
        sock.close
      end
    end
  end

  def test_connection_closed_without_greeting
    server = create_tcp_server
    port = server.addr[1]
    Thread.start do
      begin
        sock = server.accept
        sock.close
      rescue
      end
    end
    begin
      assert_raise(Net::IMAP::Error) do
        Net::IMAP.new(SERVER_ADDR, :port => port)
      end
    ensure
      server.close
    end
  end

  def test_default_port
    assert_equal(143, Net::IMAP.default_port)
    assert_equal(143, Net::IMAP.default_imap_port)
    assert_equal(993, Net::IMAP.default_tls_port)
    assert_equal(993, Net::IMAP.default_ssl_port)
    assert_equal(993, Net::IMAP.default_imaps_port)
  end

  private

  def imaps_test
    server = create_tcp_server
    port = server.addr[1]
    ctx = OpenSSL::SSL::SSLContext.new
    ctx.ca_file = CA_FILE
    ctx.key = File.open(SERVER_KEY) { |f|
      OpenSSL::PKey::RSA.new(f)
    }
    ctx.cert = File.open(SERVER_CERT) { |f|
      OpenSSL::X509::Certificate.new(f)
    }
    ssl_server = OpenSSL::SSL::SSLServer.new(server, ctx)
    Thread.start do
      begin
        sock = ssl_server.accept
        begin
          sock.print("* OK test server\r\n")
          sock.gets
          sock.print("* BYE terminating connection\r\n")
          sock.print("RUBY0001 OK LOGOUT completed\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = yield(port)
        imap.logout
      ensure
        imap.disconnect if imap
      end
    ensure
      ssl_server.close
    end
  end

  def starttls_test
    server = create_tcp_server
    port = server.addr[1]
    Thread.start do
      begin
        sock = server.accept
        sock.print("* OK test server\r\n")
        sock.gets
        sock.print("RUBY0001 OK completed\r\n")
        ctx = OpenSSL::SSL::SSLContext.new
        ctx.ca_file = CA_FILE
        ctx.key = File.open(SERVER_KEY) { |f|
          OpenSSL::PKey::RSA.new(f)
        }
        ctx.cert = File.open(SERVER_CERT) { |f|
          OpenSSL::X509::Certificate.new(f)
        }
        sock = OpenSSL::SSL::SSLSocket.new(sock, ctx)
        begin
          sock.accept
          sock.gets
          sock.print("* BYE terminating connection\r\n")
          sock.print("RUBY0002 OK LOGOUT completed\r\n")
        ensure
          sock.close
        end
      rescue
      end
    end
    begin
      begin
        imap = yield(port)
        imap.logout if !imap.disconnected?
      ensure
        imap.disconnect if imap && !imap.disconnected?
      end
    ensure
      server.close
    end
  end

  def create_tcp_server
    return TCPServer.new(SERVER_ADDR, 0)
  end
end
