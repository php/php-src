require "test/unit"
require_relative "utils.rb"
require "webrick"
require "stringio"

class WEBrick::TestFileHandler < Test::Unit::TestCase
  def default_file_handler(filename)
    klass = WEBrick::HTTPServlet::DefaultFileHandler
    klass.new(WEBrick::Config::HTTP, filename)
  end

  def windows?
    File.directory?("\\")
  end

  def get_res_body(res)
    if defined? res.body.read
      res.body.read
    else
      res.body
    end
  end

  def make_range_request(range_spec)
    msg = <<-END_OF_REQUEST
      GET / HTTP/1.0
      Range: #{range_spec}

    END_OF_REQUEST
    return StringIO.new(msg.gsub(/^ {6}/, ""))
  end

  def make_range_response(file, range_spec)
    req = WEBrick::HTTPRequest.new(WEBrick::Config::HTTP)
    req.parse(make_range_request(range_spec))
    res = WEBrick::HTTPResponse.new(WEBrick::Config::HTTP)
    size = File.size(file)
    handler = default_file_handler(file)
    handler.make_partial_content(req, res, file, size)
    return res
  end

  def test_make_partial_content
    filename = __FILE__
    filesize = File.size(filename)

    res = make_range_response(filename, "bytes=#{filesize-100}-")
    assert_match(%r{^text/plain}, res["content-type"])
    assert_equal(get_res_body(res).size, 100)

    res = make_range_response(filename, "bytes=-100")
    assert_match(%r{^text/plain}, res["content-type"])
    assert_equal(get_res_body(res).size, 100)

    res = make_range_response(filename, "bytes=0-99")
    assert_match(%r{^text/plain}, res["content-type"])
    assert_equal(get_res_body(res).size, 100)

    res = make_range_response(filename, "bytes=100-199")
    assert_match(%r{^text/plain}, res["content-type"])
    assert_equal(get_res_body(res).size, 100)

    res = make_range_response(filename, "bytes=0-0")
    assert_match(%r{^text/plain}, res["content-type"])
    assert_equal(get_res_body(res).size, 1)

    res = make_range_response(filename, "bytes=-1")
    assert_match(%r{^text/plain}, res["content-type"])
    assert_equal(get_res_body(res).size, 1)

    res = make_range_response(filename, "bytes=0-0, -2")
    assert_match(%r{^multipart/byteranges}, res["content-type"])
  end

  def test_filehandler
    config = { :DocumentRoot => File.dirname(__FILE__), }
    this_file = File.basename(__FILE__)
    filesize = File.size(__FILE__)
    this_data = File.open(__FILE__, "rb") {|f| f.read}
    range = nil
    bug2593 = '[ruby-dev:40030]'

    TestWEBrick.start_httpserver(config) do |server, addr, port, log|
      http = Net::HTTP.new(addr, port)
      req = Net::HTTP::Get.new("/")
      http.request(req){|res|
        assert_equal("200", res.code, log.call)
        assert_equal("text/html", res.content_type, log.call)
        assert_match(/HREF="#{this_file}"/, res.body, log.call)
      }
      req = Net::HTTP::Get.new("/#{this_file}")
      http.request(req){|res|
        assert_equal("200", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_equal(File.read(__FILE__), res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=#{filesize-100}-")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_nothing_raised(bug2593) {range = res.content_range}
        assert_equal((filesize-100)..(filesize-1), range, log.call)
        assert_equal(this_data[-100..-1], res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=-100")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_nothing_raised(bug2593) {range = res.content_range}
        assert_equal((filesize-100)..(filesize-1), range, log.call)
        assert_equal(this_data[-100..-1], res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=0-99")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_nothing_raised(bug2593) {range = res.content_range}
        assert_equal(0..99, range, log.call)
        assert_equal(this_data[0..99], res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=100-199")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_nothing_raised(bug2593) {range = res.content_range}
        assert_equal(100..199, range, log.call)
        assert_equal(this_data[100..199], res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=0-0")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_nothing_raised(bug2593) {range = res.content_range}
        assert_equal(0..0, range, log.call)
        assert_equal(this_data[0..0], res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=-1")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("text/plain", res.content_type, log.call)
        assert_nothing_raised(bug2593) {range = res.content_range}
        assert_equal((filesize-1)..(filesize-1), range, log.call)
        assert_equal(this_data[-1, 1], res.body, log.call)
      }

      req = Net::HTTP::Get.new("/#{this_file}", "range"=>"bytes=0-0, -2")
      http.request(req){|res|
        assert_equal("206", res.code, log.call)
        assert_equal("multipart/byteranges", res.content_type, log.call)
      }

    end
  end

  def test_non_disclosure_name
    config = { :DocumentRoot => File.dirname(__FILE__), }
    this_file = File.basename(__FILE__)
    TestWEBrick.start_httpserver(config) do |server, addr, port, log|
      http = Net::HTTP.new(addr, port)
      doc_root_opts = server[:DocumentRootOptions]
      doc_root_opts[:NondisclosureName] = %w(.ht* *~ test_*)
      req = Net::HTTP::Get.new("/")
      http.request(req){|res|
        assert_equal("200", res.code, log.call)
        assert_equal("text/html", res.content_type, log.call)
        assert_no_match(/HREF="#{File.basename(__FILE__)}"/, res.body)
      }
      req = Net::HTTP::Get.new("/#{this_file}")
      http.request(req){|res|
        assert_equal("404", res.code, log.call)
      }
      doc_root_opts[:NondisclosureName] = %w(.ht* *~ TEST_*)
      http.request(req){|res|
        assert_equal("404", res.code, log.call)
      }
    end
  end

  def test_directory_traversal
    config = { :DocumentRoot => File.dirname(__FILE__), }
    this_file = File.basename(__FILE__)
    TestWEBrick.start_httpserver(config) do |server, addr, port, log|
      http = Net::HTTP.new(addr, port)
      req = Net::HTTP::Get.new("/../../")
      http.request(req){|res| assert_equal("400", res.code, log.call) }
      req = Net::HTTP::Get.new("/..%5c../#{File.basename(__FILE__)}")
      http.request(req){|res| assert_equal(windows? ? "200" : "404", res.code, log.call) }
      req = Net::HTTP::Get.new("/..%5c..%5cruby.c")
      http.request(req){|res| assert_equal("404", res.code, log.call) }
    end
  end

  def test_unwise_in_path
    if windows?
      config = { :DocumentRoot => File.dirname(__FILE__), }
      this_file = File.basename(__FILE__)
      TestWEBrick.start_httpserver(config) do |server, addr, port, log|
        http = Net::HTTP.new(addr, port)
        req = Net::HTTP::Get.new("/..%5c..")
        http.request(req){|res| assert_equal("301", res.code, log.call) }
      end
    end
  end

  def test_short_filename
    config = {
      :CGIInterpreter => TestWEBrick::RubyBin,
      :DocumentRoot => File.dirname(__FILE__),
      :CGIPathEnv => ENV['PATH'],
    }
    TestWEBrick.start_httpserver(config) do |server, addr, port, log|
      http = Net::HTTP.new(addr, port)
      if windows?
        fname = nil
        Dir.chdir(config[:DocumentRoot]) do
          fname = IO.popen("dir /x webrick_long_filename.cgi", "r").read.match(/\s(w.+?cgi)\s/i)[1].downcase
        end
      else
        fname = "webric~1.cgi"
      end
      req = Net::HTTP::Get.new("/#{fname}/test")
      http.request(req) do |res|
        if windows?
          assert_equal("200", res.code, log.call)
          assert_equal("/test", res.body, log.call)
        else
          assert_equal("404", res.code, log.call)
        end
      end

      req = Net::HTTP::Get.new("/.htaccess")
      http.request(req) {|res| assert_equal("404", res.code, log.call) }
      req = Net::HTTP::Get.new("/htacce~1")
      http.request(req) {|res| assert_equal("404", res.code, log.call) }
      req = Net::HTTP::Get.new("/HTACCE~1")
      http.request(req) {|res| assert_equal("404", res.code, log.call) }
    end
  end

  def test_script_disclosure
    config = {
      :CGIInterpreter => TestWEBrick::RubyBin,
      :DocumentRoot => File.dirname(__FILE__),
      :CGIPathEnv => ENV['PATH'],
      :RequestCallback => Proc.new{|req, res|
        def req.meta_vars
          meta = super
          meta["RUBYLIB"] = $:.join(File::PATH_SEPARATOR)
          meta[RbConfig::CONFIG['LIBPATHENV']] = ENV[RbConfig::CONFIG['LIBPATHENV']] if RbConfig::CONFIG['LIBPATHENV']
          return meta
        end
      },
    }
    TestWEBrick.start_httpserver(config) do |server, addr, port, log|
      http = Net::HTTP.new(addr, port)

      req = Net::HTTP::Get.new("/webrick.cgi/test")
      http.request(req) do |res|
        assert_equal("200", res.code, log.call)
        assert_equal("/test", res.body, log.call)
      end

      response_assertion = Proc.new do |res|
        if windows?
          assert_equal("200", res.code, log.call)
          assert_equal("/test", res.body, log.call)
        else
          assert_equal("404", res.code, log.call)
        end
      end
      req = Net::HTTP::Get.new("/webrick.cgi%20/test")
      http.request(req, &response_assertion)
      req = Net::HTTP::Get.new("/webrick.cgi./test")
      http.request(req, &response_assertion)
      req = Net::HTTP::Get.new("/webrick.cgi::$DATA/test")
      http.request(req, &response_assertion)
    end
  end
end
