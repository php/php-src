require 'test/unit'
require 'uri/ftp'

module URI


class TestFTP < Test::Unit::TestCase
  def setup
  end

  def test_parse
    url = URI.parse('ftp://user:pass@host.com/abc/def')
    assert_kind_of(URI::FTP, url)

    exp = [
      'ftp',
      'user:pass', 'host.com', URI::FTP.default_port,
      'abc/def', nil,
    ]
    ary = [
      url.scheme, url.userinfo, url.host, url.port,
      url.path, url.opaque
    ]
    assert_equal(exp, ary)

    assert_equal('user', url.user)
    assert_equal('pass', url.password)
  end

  def test_paths
    # If you think what's below is wrong, please read RubyForge bug 2055,
    # RFC 1738 section 3.2.2, and RFC 2396.
    u = URI.parse('ftp://ftp.example.com/foo/bar/file.ext')
    assert(u.path == 'foo/bar/file.ext')
    u = URI.parse('ftp://ftp.example.com//foo/bar/file.ext')
    assert(u.path == '/foo/bar/file.ext')
    u = URI.parse('ftp://ftp.example.com/%2Ffoo/bar/file.ext')
    assert(u.path == '/foo/bar/file.ext')
  end

  def test_assemble
    # uri/ftp is conservative and uses the older RFC 1738 rules, rather than
    # assuming everyone else has implemented RFC 2396.
    uri = URI::FTP.build(['user:password', 'ftp.example.com', nil,
                         '/path/file.zip', 'i'])
    assert(uri.to_s ==
           'ftp://user:password@ftp.example.com/%2Fpath/file.zip;type=i')
  end

  def test_select
    assert_equal(['ftp', 'a.b.c', 21], URI.parse('ftp://a.b.c/').select(:scheme, :host, :port))
    u = URI.parse('ftp://a.b.c/')
    ary = u.component.collect {|c| u.send(c)}
    assert_equal(ary, u.select(*u.component))
    assert_raise(ArgumentError) do
      u.select(:scheme, :host, :not_exist, :port)
    end
  end
end


end
