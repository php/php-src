require_relative "utils"
require "webrick"
require "test/unit"

class TestWEBrickCGI < Test::Unit::TestCase
  CRLF = "\r\n"

  def start_cgi_server(&block)
    config = {
      :CGIInterpreter => TestWEBrick::RubyBin,
      :DocumentRoot => File.dirname(__FILE__),
      :DirectoryIndex => ["webrick.cgi"],
      :RequestCallback => Proc.new{|req, res|
        def req.meta_vars
          meta = super
          meta["RUBYLIB"] = $:.join(File::PATH_SEPARATOR)
          meta[RbConfig::CONFIG['LIBPATHENV']] = ENV[RbConfig::CONFIG['LIBPATHENV']] if RbConfig::CONFIG['LIBPATHENV']
          return meta
        end
      },
    }
    if RUBY_PLATFORM =~ /mswin32|mingw|cygwin|bccwin32/
      config[:CGIPathEnv] = ENV['PATH'] # runtime dll may not be in system dir.
    end
    TestWEBrick.start_httpserver(config){|server, addr, port, log|
      block.call(server, addr, port, log)
    }
  end

  def test_cgi
    start_cgi_server{|server, addr, port, log|
      http = Net::HTTP.new(addr, port)
      req = Net::HTTP::Get.new("/webrick.cgi")
      http.request(req){|res| assert_equal("/webrick.cgi", res.body, log.call)}
      req = Net::HTTP::Get.new("/webrick.cgi/path/info")
      http.request(req){|res| assert_equal("/path/info", res.body, log.call)}
      req = Net::HTTP::Get.new("/webrick.cgi/%3F%3F%3F?foo=bar")
      http.request(req){|res| assert_equal("/???", res.body, log.call)}
      req = Net::HTTP::Get.new("/webrick.cgi/%A4%DB%A4%B2/%A4%DB%A4%B2")
      http.request(req){|res|
        assert_equal("/\xA4\xDB\xA4\xB2/\xA4\xDB\xA4\xB2", res.body, log.call)}
      req = Net::HTTP::Get.new("/webrick.cgi?a=1;a=2;b=x")
      http.request(req){|res| assert_equal("a=1, a=2, b=x", res.body, log.call)}
      req = Net::HTTP::Get.new("/webrick.cgi?a=1&a=2&b=x")
      http.request(req){|res| assert_equal("a=1, a=2, b=x", res.body, log.call)}

      req = Net::HTTP::Post.new("/webrick.cgi?a=x;a=y;b=1")
      req["Content-Type"] = "application/x-www-form-urlencoded"
      http.request(req, "a=1;a=2;b=x"){|res|
        assert_equal("a=1, a=2, b=x", res.body, log.call)}
      req = Net::HTTP::Post.new("/webrick.cgi?a=x&a=y&b=1")
      req["Content-Type"] = "application/x-www-form-urlencoded"
      http.request(req, "a=1&a=2&b=x"){|res|
        assert_equal("a=1, a=2, b=x", res.body, log.call)}
      req = Net::HTTP::Get.new("/")
      http.request(req){|res|
        ary = res.body.lines.to_a
        assert_match(%r{/$}, ary[0], log.call)
        assert_match(%r{/webrick.cgi$}, ary[1], log.call)
      }

      req = Net::HTTP::Get.new("/webrick.cgi")
      req["Cookie"] = "CUSTOMER=WILE_E_COYOTE; PART_NUMBER=ROCKET_LAUNCHER_0001"
      http.request(req){|res|
        assert_equal(
          "CUSTOMER=WILE_E_COYOTE\nPART_NUMBER=ROCKET_LAUNCHER_0001\n",
          res.body, log.call)
      }

      req = Net::HTTP::Get.new("/webrick.cgi")
      cookie =  %{$Version="1"; }
      cookie << %{Customer="WILE_E_COYOTE"; $Path="/acme"; }
      cookie << %{Part_Number="Rocket_Launcher_0001"; $Path="/acme"; }
      cookie << %{Shipping="FedEx"; $Path="/acme"}
      req["Cookie"] = cookie
      http.request(req){|res|
        assert_equal("Customer=WILE_E_COYOTE, Shipping=FedEx",
                     res["Set-Cookie"], log.call)
        assert_equal("Customer=WILE_E_COYOTE\n" +
                     "Part_Number=Rocket_Launcher_0001\n" +
                     "Shipping=FedEx\n", res.body, log.call)
      }
    }
  end

  def test_bad_request
    start_cgi_server{|server, addr, port, log|
      sock = TCPSocket.new(addr, port)
      begin
        sock << "POST /webrick.cgi HTTP/1.0" << CRLF
        sock << "Content-Type: application/x-www-form-urlencoded" << CRLF
        sock << "Content-Length: 1024" << CRLF
        sock << CRLF
        sock << "a=1&a=2&b=x"
        sock.close_write
        assert_match(%r{\AHTTP/\d.\d 400 Bad Request}, sock.read, log.call)
      ensure
        sock.close
      end
    }
  end

  CtrlSeq = [0x7f, *(1..31)].pack("C*").gsub(/\s+/, '')
  CtrlPat = /#{Regexp.quote(CtrlSeq)}/o
  DumpPat = /#{Regexp.quote(CtrlSeq.dump[1...-1])}/o

  def test_bad_uri
    start_cgi_server{|server, addr, port, log|
      res = TCPSocket.open(addr, port) {|sock|
        sock << "GET /#{CtrlSeq}#{CRLF}#{CRLF}"
        sock.close_write
        sock.read
      }
      assert_match(%r{\AHTTP/\d.\d 400 Bad Request}, res)
      s = log.call.each_line.grep(/ERROR bad URI/)[0]
      assert_match(DumpPat, s)
      assert_not_match(CtrlPat, s)
    }
  end

  def test_bad_header
    start_cgi_server{|server, addr, port, log|
      res = TCPSocket.open(addr, port) {|sock|
        sock << "GET / HTTP/1.0#{CRLF}#{CtrlSeq}#{CRLF}#{CRLF}"
        sock.close_write
        sock.read
      }
      assert_match(%r{\AHTTP/\d.\d 400 Bad Request}, res)
      s = log.call.each_line.grep(/ERROR bad header/)[0]
      assert_match(DumpPat, s)
      assert_not_match(CtrlPat, s)
    }
  end
end
