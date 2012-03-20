require 'test/unit'
require 'time'
require 'webrick'
require_relative 'webrick_testing'
require "xmlrpc/server"
require 'xmlrpc/client'

class TestCookie < Test::Unit::TestCase
  include WEBrick_Testing

  def create_servlet
    s = XMLRPC::WEBrickServlet.new

    def s.logged_in_users
      @logged_in_users ||= {}
    end
    def s.request
      @request
    end
    def s.response
      @response
    end
    def s.service(request, response)
      @request = request
      @response = response
      super
    ensure
      @request = nil
      @response = nil
    end

    key = Time.now.to_i.to_s
    valid_user = "valid-user"
    s.add_handler("test.login") do |user, password|
      ok = (user == valid_user and password == "secret")
      if ok
        s.logged_in_users[key] = user
        expires = (Time.now + 60 * 60).httpdate
        cookies = s.response.cookies
        cookies << "key=\"#{key}\"; path=\"/RPC2\"; expires=#{expires}"
        cookies << "user=\"#{user}\"; path=\"/RPC2\""
      end
      ok
    end

    s.add_handler("test.require_authenticate_echo") do |string|
      cookies = {}
      s.request.cookies.each do |cookie|
        cookies[cookie.name] = cookie.value
      end
      if cookies == {"key" => key, "user" => valid_user}
        string
      else
        raise XMLRPC::FaultException.new(29, "Authentication required")
      end
    end

    s.set_default_handler do |name, *args|
      raise XMLRPC::FaultException.new(-99, "Method #{name} missing" +
            " or wrong number of parameters!")
    end

    s.add_introspection

    s
  end

  def setup_http_server(port)
    option = {:Port => port}

    start_server(option) {|w| w.mount('/RPC2', create_servlet) }

    @s = XMLRPC::Client.new3(:port => port)
  end

  PORT = 8070
  def test_cookie
    begin
      setup_http_server(PORT)
      do_test
    ensure
      stop_server
    end
  end

  def do_test
    assert(!@s.call("test.login", "invalid-user", "invalid-password"))
    exception = assert_raise(XMLRPC::FaultException) do
      @s.call("test.require_authenticate_echo", "Hello")
    end
    assert_equal(29, exception.faultCode)

    assert(@s.call("test.login", "valid-user", "secret"))
    assert_equal("Hello", @s.call("test.require_authenticate_echo", "Hello"))
  end
end
