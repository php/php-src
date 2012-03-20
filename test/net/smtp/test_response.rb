require 'net/smtp'
require 'minitest/autorun'

module Net
  class SMTP
    class TestResponse < MiniTest::Unit::TestCase
      def test_capabilities
        res = Response.parse("250-ubuntu-desktop\n250-PIPELINING\n250-SIZE 10240000\n250-VRFY\n250-ETRN\n250-STARTTLS\n250-ENHANCEDSTATUSCODES\n250 DSN\n")

        capabilities = res.capabilities
        %w{ PIPELINING SIZE VRFY STARTTLS ENHANCEDSTATUSCODES DSN}.each do |str|
          assert capabilities.key?(str), str
        end
      end

      def test_capabilities_default
        res = Response.parse("250-ubuntu-desktop\n250-PIPELINING\n250 DSN\n")
        assert_equal [], res.capabilities['PIPELINING']
      end

      def test_capabilities_value
        res = Response.parse("250-ubuntu-desktop\n250-SIZE 1234\n250 DSN\n")
        assert_equal ['1234'], res.capabilities['SIZE']
      end

      def test_capabilities_multi
        res = Response.parse("250-ubuntu-desktop\n250-SIZE 1 2 3\n250 DSN\n")
        assert_equal %w{1 2 3}, res.capabilities['SIZE']
      end

      def test_bad_string
        res = Response.parse("badstring")
        assert_equal({}, res.capabilities)
      end

      def test_success?
        res = Response.parse("250-ubuntu-desktop\n250-SIZE 1 2 3\n250 DSN\n")
        assert res.success?
        assert !res.continue?
      end

      # RFC 2821, Section 4.2.1
      def test_continue?
        res = Response.parse("3yz-ubuntu-desktop\n250-SIZE 1 2 3\n250 DSN\n")
        assert !res.success?
        assert res.continue?
      end

      def test_status_type_char
        res = Response.parse("3yz-ubuntu-desktop\n250-SIZE 1 2 3\n250 DSN\n")
        assert_equal '3', res.status_type_char

        res = Response.parse("250-ubuntu-desktop\n250-SIZE 1 2 3\n250 DSN\n")
        assert_equal '2', res.status_type_char
      end

      def test_message
        res = Response.parse("250-ubuntu-desktop\n250-SIZE 1 2 3\n250 DSN\n")
        assert_equal "250-ubuntu-desktop\n", res.message
      end

      def test_server_busy_exception
        res = Response.parse("400 omg busy")
        assert_equal Net::SMTPServerBusy, res.exception_class
        res = Response.parse("410 omg busy")
        assert_equal Net::SMTPServerBusy, res.exception_class
      end

      def test_syntax_error_exception
        res = Response.parse("500 omg syntax error")
        assert_equal Net::SMTPSyntaxError, res.exception_class

        res = Response.parse("501 omg syntax error")
        assert_equal Net::SMTPSyntaxError, res.exception_class
      end

      def test_authentication_exception
        res = Response.parse("530 omg auth error")
        assert_equal Net::SMTPAuthenticationError, res.exception_class

        res = Response.parse("531 omg auth error")
        assert_equal Net::SMTPAuthenticationError, res.exception_class
      end

      def test_fatal_error
        res = Response.parse("510 omg fatal error")
        assert_equal Net::SMTPFatalError, res.exception_class

        res = Response.parse("511 omg fatal error")
        assert_equal Net::SMTPFatalError, res.exception_class
      end

      def test_default_exception
        res = Response.parse("250 omg fatal error")
        assert_equal Net::SMTPUnknownError, res.exception_class
      end
    end
  end
end
