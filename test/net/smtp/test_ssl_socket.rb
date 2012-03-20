require 'net/smtp'
require 'minitest/autorun'

module Net
  class TestSSLSocket < MiniTest::Unit::TestCase
    class MySMTP < SMTP
      attr_accessor :fake_tcp, :fake_ssl

      def tcp_socket address, port
        fake_tcp
      end

      def ssl_socket socket, context
        fake_ssl
      end
    end

    require 'stringio'
    class SSLSocket < StringIO
      attr_accessor :sync_close, :connected, :closed

      def initialize(*args)
        @connected = false
        @closed    = true
        super
      end

      def connect
        self.connected = true
        self.closed = false
      end

      def close
        self.closed = true
      end

      def post_connection_check omg
      end
    end

    def test_ssl_socket_close_on_post_connection_check_fail
      tcp_socket = StringIO.new success_response

      ssl_socket = SSLSocket.new.extend Module.new {
        def post_connection_check omg
          raise OpenSSL::SSL::SSLError, 'hostname was not match with the server certificate'
        end
      }

      connection = MySMTP.new('localhost', 25)
      connection.enable_starttls_auto
      connection.fake_tcp = tcp_socket
      connection.fake_ssl = ssl_socket

      assert_raises(OpenSSL::SSL::SSLError) do
        connection.start
      end
      assert_equal true, ssl_socket.closed
    end

    def test_ssl_socket_open_on_post_connection_check_success
      tcp_socket = StringIO.new success_response

      ssl_socket = SSLSocket.new success_response

      connection = MySMTP.new('localhost', 25)
      connection.enable_starttls_auto
      connection.fake_tcp = tcp_socket
      connection.fake_ssl = ssl_socket

      connection.start
      assert_equal false, ssl_socket.closed
    end

    def success_response
      [
        '220 smtp.example.com ESMTP Postfix',
        "250-ubuntu-desktop",
        "250-PIPELINING",
        "250-SIZE 10240000",
        "250-VRFY",
        "250-ETRN",
        "250-STARTTLS",
        "250-ENHANCEDSTATUSCODES",
        "250-8BITMIME",
        "250 DSN",
        "220 2.0.0 Ready to start TLS",
      ].join("\r\n") + "\r\n"
    end
  end
end if defined?(OpenSSL)
