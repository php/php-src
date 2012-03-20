require 'test/unit'
require 'uri'

class URI::TestGeneric < Test::Unit::TestCase
  def setup
    @url = 'http://a/b/c/d;p?q'
    @base_url = URI.parse(@url)
  end

  def teardown
  end

  def uri_to_ary(uri)
    uri.class.component.collect {|c| uri.send(c)}
  end

  def test_parse
    # 0
    assert_kind_of(URI::HTTP, @base_url)

    exp = [
      'http',
      nil, 'a', URI::HTTP.default_port,
      '/b/c/d;p',
      'q',
      nil
    ]
    ary = uri_to_ary(@base_url)
    assert_equal(exp, ary)

    # 1
    url = URI.parse('ftp://ftp.is.co.za/rfc/rfc1808.txt')
    assert_kind_of(URI::FTP, url)

    exp = [
      'ftp',
      nil, 'ftp.is.co.za', URI::FTP.default_port,
      'rfc/rfc1808.txt', nil,
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)
    # 1'
    url = URI.parse('ftp://ftp.is.co.za/%2Frfc/rfc1808.txt')
    assert_kind_of(URI::FTP, url)

    exp = [
      'ftp',
      nil, 'ftp.is.co.za', URI::FTP.default_port,
      '/rfc/rfc1808.txt', nil,
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)

    # 2
    url = URI.parse('gopher://spinaltap.micro.umn.edu/00/Weather/California/Los%20Angeles')
    assert_kind_of(URI::Generic, url)

    exp = [
      'gopher',
      nil, 'spinaltap.micro.umn.edu', nil, nil,
      '/00/Weather/California/Los%20Angeles', nil,
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)

    # 3
    url = URI.parse('http://www.math.uio.no/faq/compression-faq/part1.html')
    assert_kind_of(URI::HTTP, url)

    exp = [
      'http',
      nil, 'www.math.uio.no', URI::HTTP.default_port,
      '/faq/compression-faq/part1.html',
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)

    # 4
    url = URI.parse('mailto:mduerst@ifi.unizh.ch')
    assert_kind_of(URI::Generic, url)

    exp = [
      'mailto',
      'mduerst@ifi.unizh.ch',
      []
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)

    # 5
    url = URI.parse('news:comp.infosystems.www.servers.unix')
    assert_kind_of(URI::Generic, url)

    exp = [
      'news',
      nil, nil, nil, nil,
      nil, 'comp.infosystems.www.servers.unix',
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)

    # 6
    url = URI.parse('telnet://melvyl.ucop.edu/')
    assert_kind_of(URI::Generic, url)

    exp = [
      'telnet',
      nil, 'melvyl.ucop.edu', nil, nil,
      '/', nil,
      nil,
      nil
    ]
    ary = uri_to_ary(url)
    assert_equal(exp, ary)

    # 7
    # reported by Mr. Kubota <em6t-kbt@asahi-net.or.jp>
    assert_raise(URI::InvalidURIError) { URI.parse('http://a_b:80/') }
    assert_raise(URI::InvalidURIError) { URI.parse('http://a_b/') }

    # 8
    # reported by m_seki
    uri = URI.parse('file:///foo/bar.txt')
    assert_kind_of(URI::Generic, url)
    uri = URI.parse('file:/foo/bar.txt')
    assert_kind_of(URI::Generic, url)

    # 9
    url = URI.parse('ftp://:pass@localhost/')
    assert_equal('', url.user, "[ruby-dev:25667]")
    assert_equal('pass', url.password)
    assert_equal(':pass', url.userinfo, "[ruby-dev:25667]")
    url = URI.parse('ftp://user@localhost/')
    assert_equal('user', url.user)
    assert_equal(nil, url.password)
    assert_equal('user', url.userinfo)
    url = URI.parse('ftp://localhost/')
    assert_equal(nil, url.user)
    assert_equal(nil, url.password)
    assert_equal(nil, url.userinfo)
  end

  def test_merge
    u1 = URI.parse('http://foo')
    u2 = URI.parse('http://foo/')
    u3 = URI.parse('http://foo/bar')
    u4 = URI.parse('http://foo/bar/')

    assert_equal(URI.parse('http://foo/baz'), u1 + 'baz')
    assert_equal(URI.parse('http://foo/baz'), u2 + 'baz')
    assert_equal(URI.parse('http://foo/baz'), u3 + 'baz')
    assert_equal(URI.parse('http://foo/bar/baz'), u4 + 'baz')

    assert_equal(URI.parse('http://foo/baz'), u1 + '/baz')
    assert_equal(URI.parse('http://foo/baz'), u2 + '/baz')
    assert_equal(URI.parse('http://foo/baz'), u3 + '/baz')
    assert_equal(URI.parse('http://foo/baz'), u4 + '/baz')

    url = URI.parse('http://hoge/a.html') + 'b.html'
    assert_equal('http://hoge/b.html', url.to_s, "[ruby-dev:11508]")

    # reported by Mr. Kubota <em6t-kbt@asahi-net.or.jp>
    url = URI.parse('http://a/b') + 'http://x/y'
    assert_equal('http://x/y', url.to_s)
    assert_equal(url, URI.parse('')                     + 'http://x/y')
    assert_equal(url, URI.parse('').normalize           + 'http://x/y')
    assert_equal(url, URI.parse('http://a/b').normalize + 'http://x/y')

    u = URI.parse('http://foo/bar/baz')
    assert_equal(nil, u.merge!(""))
    assert_equal(nil, u.merge!(u))
    assert(nil != u.merge!("."))
    assert_equal('http://foo/bar/', u.to_s)
    assert(nil != u.merge!("../baz"))
    assert_equal('http://foo/baz', u.to_s)

    u0 = URI.parse('mailto:foo@example.com')
    u1 = URI.parse('mailto:foo@example.com#bar')
    assert_equal(uri_to_ary(u0 + '#bar'), uri_to_ary(u1), "[ruby-dev:23628]")

    u0 = URI.parse('http://www.example.com/')
    u1 = URI.parse('http://www.example.com/foo/..') + './'
    assert_equal(u0, u1, "[ruby-list:39838]")
    u0 = URI.parse('http://www.example.com/foo/')
    u1 = URI.parse('http://www.example.com/foo/bar/..') + './'
    assert_equal(u0, u1)
    u0 = URI.parse('http://www.example.com/foo/bar/')
    u1 = URI.parse('http://www.example.com/foo/bar/baz/..') + './'
    assert_equal(u0, u1)
    u0 = URI.parse('http://www.example.com/')
    u1 = URI.parse('http://www.example.com/foo/bar/../..') + './'
    assert_equal(u0, u1)
    u0 = URI.parse('http://www.example.com/foo/')
    u1 = URI.parse('http://www.example.com/foo/bar/baz/../..') + './'
    assert_equal(u0, u1)

    u = URI.parse('http://www.example.com/')
    u0 = u + './foo/'
    u1 = u + './foo/bar/..'
    assert_equal(u0, u1, "[ruby-list:39844]")
    u = URI.parse('http://www.example.com/')
    u0 = u + './'
    u1 = u + './foo/bar/../..'
    assert_equal(u0, u1)
  end

  def test_route
    url = URI.parse('http://hoge/a.html').route_to('http://hoge/b.html')
    assert_equal('b.html', url.to_s)

    url = URI.parse('http://hoge/a/').route_to('http://hoge/b/')
    assert_equal('../b/', url.to_s)
    url = URI.parse('http://hoge/a/b').route_to('http://hoge/b/')
    assert_equal('../b/', url.to_s)

    url = URI.parse('http://hoge/a/b/').route_to('http://hoge/b/')
    assert_equal('../../b/', url.to_s)

    url = URI.parse('http://hoge/a/b/').route_to('http://HOGE/b/')
    assert_equal('../../b/', url.to_s)

    url = URI.parse('http://hoge/a/b/').route_to('http://MOGE/b/')
    assert_equal('//MOGE/b/', url.to_s)

    url = URI.parse('http://hoge/b').route_to('http://hoge/b/')
    assert_equal('b/', url.to_s)
    url = URI.parse('http://hoge/b/a').route_to('http://hoge/b/')
    assert_equal('./', url.to_s)
    url = URI.parse('http://hoge/b/').route_to('http://hoge/b')
    assert_equal('../b', url.to_s)
    url = URI.parse('http://hoge/b').route_to('http://hoge/b:c')
    assert_equal('./b:c', url.to_s)

    url = URI.parse('file:///a/b/').route_to('file:///a/b/')
    assert_equal('', url.to_s)
    url = URI.parse('file:///a/b/').route_to('file:///a/b')
    assert_equal('../b', url.to_s)

    url = URI.parse('mailto:foo@example.com').route_to('mailto:foo@example.com#bar')
    assert_equal('#bar', url.to_s)

    url = URI.parse('mailto:foo@example.com#bar').route_to('mailto:foo@example.com')
    assert_equal('', url.to_s)

    url = URI.parse('mailto:foo@example.com').route_to('mailto:foo@example.com')
    assert_equal('', url.to_s)
  end

  def test_rfc3986_examples
#  http://a/b/c/d;p?q
#        g:h           =  g:h
    url = @base_url.merge('g:h')
    assert_kind_of(URI::Generic, url)
    assert_equal('g:h', url.to_s)
    url = @base_url.route_to('g:h')
    assert_kind_of(URI::Generic, url)
    assert_equal('g:h', url.to_s)

#  http://a/b/c/d;p?q
#        g             =  http://a/b/c/g
    url = @base_url.merge('g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g', url.to_s)
    url = @base_url.route_to('http://a/b/c/g')
    assert_kind_of(URI::Generic, url)
    assert_equal('g', url.to_s)

#  http://a/b/c/d;p?q
#        ./g           =  http://a/b/c/g
    url = @base_url.merge('./g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g', url.to_s)
    url = @base_url.route_to('http://a/b/c/g')
    assert_kind_of(URI::Generic, url)
    assert('./g' != url.to_s) # ok
    assert_equal('g', url.to_s)

#  http://a/b/c/d;p?q
#        g/            =  http://a/b/c/g/
    url = @base_url.merge('g/')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g/', url.to_s)
    url = @base_url.route_to('http://a/b/c/g/')
    assert_kind_of(URI::Generic, url)
    assert_equal('g/', url.to_s)

#  http://a/b/c/d;p?q
#        /g            =  http://a/g
    url = @base_url.merge('/g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/g', url.to_s)
    url = @base_url.route_to('http://a/g')
    assert_kind_of(URI::Generic, url)
    assert('/g' != url.to_s) # ok
    assert_equal('../../g', url.to_s)

#  http://a/b/c/d;p?q
#        //g           =  http://g
    url = @base_url.merge('//g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://g', url.to_s)
    url = @base_url.route_to('http://g')
    assert_kind_of(URI::Generic, url)
    assert_equal('//g', url.to_s)

#  http://a/b/c/d;p?q
#        ?y            =  http://a/b/c/d;p?y
    url = @base_url.merge('?y')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/d;p?y', url.to_s)
    url = @base_url.route_to('http://a/b/c/d;p?y')
    assert_kind_of(URI::Generic, url)
    assert_equal('?y', url.to_s)

#  http://a/b/c/d;p?q
#        g?y           =  http://a/b/c/g?y
    url = @base_url.merge('g?y')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g?y', url.to_s)
    url = @base_url.route_to('http://a/b/c/g?y')
    assert_kind_of(URI::Generic, url)
    assert_equal('g?y', url.to_s)

#  http://a/b/c/d;p?q
#        #s            =  http://a/b/c/d;p?q#s
    url = @base_url.merge('#s')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/d;p?q#s', url.to_s)
    url = @base_url.route_to('http://a/b/c/d;p?q#s')
    assert_kind_of(URI::Generic, url)
    assert_equal('#s', url.to_s)

#  http://a/b/c/d;p?q
#        g#s           =  http://a/b/c/g#s
    url = @base_url.merge('g#s')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g#s', url.to_s)
    url = @base_url.route_to('http://a/b/c/g#s')
    assert_kind_of(URI::Generic, url)
    assert_equal('g#s', url.to_s)

#  http://a/b/c/d;p?q
#        g?y#s         =  http://a/b/c/g?y#s
    url = @base_url.merge('g?y#s')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g?y#s', url.to_s)
    url = @base_url.route_to('http://a/b/c/g?y#s')
    assert_kind_of(URI::Generic, url)
    assert_equal('g?y#s', url.to_s)

#  http://a/b/c/d;p?q
#        ;x            =  http://a/b/c/;x
    url = @base_url.merge(';x')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/;x', url.to_s)
    url = @base_url.route_to('http://a/b/c/;x')
    assert_kind_of(URI::Generic, url)
    assert_equal(';x', url.to_s)

#  http://a/b/c/d;p?q
#        g;x           =  http://a/b/c/g;x
    url = @base_url.merge('g;x')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g;x', url.to_s)
    url = @base_url.route_to('http://a/b/c/g;x')
    assert_kind_of(URI::Generic, url)
    assert_equal('g;x', url.to_s)

#  http://a/b/c/d;p?q
#        g;x?y#s       =  http://a/b/c/g;x?y#s
    url = @base_url.merge('g;x?y#s')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g;x?y#s', url.to_s)
    url = @base_url.route_to('http://a/b/c/g;x?y#s')
    assert_kind_of(URI::Generic, url)
    assert_equal('g;x?y#s', url.to_s)

#  http://a/b/c/d;p?q
#        .             =  http://a/b/c/
    url = @base_url.merge('.')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/', url.to_s)
    url = @base_url.route_to('http://a/b/c/')
    assert_kind_of(URI::Generic, url)
    assert('.' != url.to_s) # ok
    assert_equal('./', url.to_s)

#  http://a/b/c/d;p?q
#        ./            =  http://a/b/c/
    url = @base_url.merge('./')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/', url.to_s)
    url = @base_url.route_to('http://a/b/c/')
    assert_kind_of(URI::Generic, url)
    assert_equal('./', url.to_s)

#  http://a/b/c/d;p?q
#        ..            =  http://a/b/
    url = @base_url.merge('..')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/', url.to_s)
    url = @base_url.route_to('http://a/b/')
    assert_kind_of(URI::Generic, url)
    assert('..' != url.to_s) # ok
    assert_equal('../', url.to_s)

#  http://a/b/c/d;p?q
#        ../           =  http://a/b/
    url = @base_url.merge('../')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/', url.to_s)
    url = @base_url.route_to('http://a/b/')
    assert_kind_of(URI::Generic, url)
    assert_equal('../', url.to_s)

#  http://a/b/c/d;p?q
#        ../g          =  http://a/b/g
    url = @base_url.merge('../g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/g', url.to_s)
    url = @base_url.route_to('http://a/b/g')
    assert_kind_of(URI::Generic, url)
    assert_equal('../g', url.to_s)

#  http://a/b/c/d;p?q
#        ../..         =  http://a/
    url = @base_url.merge('../..')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/', url.to_s)
    url = @base_url.route_to('http://a/')
    assert_kind_of(URI::Generic, url)
    assert('../..' != url.to_s) # ok
    assert_equal('../../', url.to_s)

#  http://a/b/c/d;p?q
#        ../../        =  http://a/
    url = @base_url.merge('../../')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/', url.to_s)
    url = @base_url.route_to('http://a/')
    assert_kind_of(URI::Generic, url)
    assert_equal('../../', url.to_s)

#  http://a/b/c/d;p?q
#        ../../g       =  http://a/g
    url = @base_url.merge('../../g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/g', url.to_s)
    url = @base_url.route_to('http://a/g')
    assert_kind_of(URI::Generic, url)
    assert_equal('../../g', url.to_s)

#  http://a/b/c/d;p?q
#        <>            =  (current document)
    url = @base_url.merge('')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/d;p?q', url.to_s)
    url = @base_url.route_to('http://a/b/c/d;p?q')
    assert_kind_of(URI::Generic, url)
    assert_equal('', url.to_s)

#  http://a/b/c/d;p?q
#        /./g          =  http://a/g
    url = @base_url.merge('/./g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/g', url.to_s)
#    url = @base_url.route_to('http://a/./g')
#    assert_kind_of(URI::Generic, url)
#    assert_equal('/./g', url.to_s)

#  http://a/b/c/d;p?q
#        /../g         =  http://a/g
    url = @base_url.merge('/../g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/g', url.to_s)
#    url = @base_url.route_to('http://a/../g')
#    assert_kind_of(URI::Generic, url)
#    assert_equal('/../g', url.to_s)

#  http://a/b/c/d;p?q
#        g.            =  http://a/b/c/g.
    url = @base_url.merge('g.')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g.', url.to_s)
    url = @base_url.route_to('http://a/b/c/g.')
    assert_kind_of(URI::Generic, url)
    assert_equal('g.', url.to_s)

#  http://a/b/c/d;p?q
#        .g            =  http://a/b/c/.g
    url = @base_url.merge('.g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/.g', url.to_s)
    url = @base_url.route_to('http://a/b/c/.g')
    assert_kind_of(URI::Generic, url)
    assert_equal('.g', url.to_s)

#  http://a/b/c/d;p?q
#        g..           =  http://a/b/c/g..
    url = @base_url.merge('g..')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g..', url.to_s)
    url = @base_url.route_to('http://a/b/c/g..')
    assert_kind_of(URI::Generic, url)
    assert_equal('g..', url.to_s)

#  http://a/b/c/d;p?q
#        ..g           =  http://a/b/c/..g
    url = @base_url.merge('..g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/..g', url.to_s)
    url = @base_url.route_to('http://a/b/c/..g')
    assert_kind_of(URI::Generic, url)
    assert_equal('..g', url.to_s)

#  http://a/b/c/d;p?q
#        ../../../g    =  http://a/g
    url = @base_url.merge('../../../g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/g', url.to_s)
    url = @base_url.route_to('http://a/g')
    assert_kind_of(URI::Generic, url)
    assert('../../../g' != url.to_s)  # ok? yes, it confuses you
    assert_equal('../../g', url.to_s) # and it is clearly

#  http://a/b/c/d;p?q
#        ../../../../g =  http://a/g
    url = @base_url.merge('../../../../g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/g', url.to_s)
    url = @base_url.route_to('http://a/g')
    assert_kind_of(URI::Generic, url)
    assert('../../../../g' != url.to_s) # ok? yes, it confuses you
    assert_equal('../../g', url.to_s)   # and it is clearly

#  http://a/b/c/d;p?q
#        ./../g        =  http://a/b/g
    url = @base_url.merge('./../g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/g', url.to_s)
    url = @base_url.route_to('http://a/b/g')
    assert_kind_of(URI::Generic, url)
    assert('./../g' != url.to_s) # ok
    assert_equal('../g', url.to_s)

#  http://a/b/c/d;p?q
#        ./g/.         =  http://a/b/c/g/
    url = @base_url.merge('./g/.')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g/', url.to_s)
    url = @base_url.route_to('http://a/b/c/g/')
    assert_kind_of(URI::Generic, url)
    assert('./g/.' != url.to_s) # ok
    assert_equal('g/', url.to_s)

#  http://a/b/c/d;p?q
#        g/./h         =  http://a/b/c/g/h
    url = @base_url.merge('g/./h')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g/h', url.to_s)
    url = @base_url.route_to('http://a/b/c/g/h')
    assert_kind_of(URI::Generic, url)
    assert('g/./h' != url.to_s) # ok
    assert_equal('g/h', url.to_s)

#  http://a/b/c/d;p?q
#        g/../h        =  http://a/b/c/h
    url = @base_url.merge('g/../h')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/h', url.to_s)
    url = @base_url.route_to('http://a/b/c/h')
    assert_kind_of(URI::Generic, url)
    assert('g/../h' != url.to_s) # ok
    assert_equal('h', url.to_s)

#  http://a/b/c/d;p?q
#        g;x=1/./y     =  http://a/b/c/g;x=1/y
    url = @base_url.merge('g;x=1/./y')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g;x=1/y', url.to_s)
    url = @base_url.route_to('http://a/b/c/g;x=1/y')
    assert_kind_of(URI::Generic, url)
    assert('g;x=1/./y' != url.to_s) # ok
    assert_equal('g;x=1/y', url.to_s)

#  http://a/b/c/d;p?q
#        g;x=1/../y    =  http://a/b/c/y
    url = @base_url.merge('g;x=1/../y')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/y', url.to_s)
    url = @base_url.route_to('http://a/b/c/y')
    assert_kind_of(URI::Generic, url)
    assert('g;x=1/../y' != url.to_s) # ok
    assert_equal('y', url.to_s)

#  http://a/b/c/d;p?q
#        g?y/./x       =  http://a/b/c/g?y/./x
    url = @base_url.merge('g?y/./x')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g?y/./x', url.to_s)
    url = @base_url.route_to('http://a/b/c/g?y/./x')
    assert_kind_of(URI::Generic, url)
    assert_equal('g?y/./x', url.to_s)

#  http://a/b/c/d;p?q
#        g?y/../x      =  http://a/b/c/g?y/../x
    url = @base_url.merge('g?y/../x')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g?y/../x', url.to_s)
    url = @base_url.route_to('http://a/b/c/g?y/../x')
    assert_kind_of(URI::Generic, url)
    assert_equal('g?y/../x', url.to_s)

#  http://a/b/c/d;p?q
#        g#s/./x       =  http://a/b/c/g#s/./x
    url = @base_url.merge('g#s/./x')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g#s/./x', url.to_s)
    url = @base_url.route_to('http://a/b/c/g#s/./x')
    assert_kind_of(URI::Generic, url)
    assert_equal('g#s/./x', url.to_s)

#  http://a/b/c/d;p?q
#        g#s/../x      =  http://a/b/c/g#s/../x
    url = @base_url.merge('g#s/../x')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http://a/b/c/g#s/../x', url.to_s)
    url = @base_url.route_to('http://a/b/c/g#s/../x')
    assert_kind_of(URI::Generic, url)
    assert_equal('g#s/../x', url.to_s)

#  http://a/b/c/d;p?q
#        http:g        =  http:g           ; for validating parsers
#                      |  http://a/b/c/g   ; for backwards compatibility
    url = @base_url.merge('http:g')
    assert_kind_of(URI::HTTP, url)
    assert_equal('http:g', url.to_s)
    url = @base_url.route_to('http:g')
    assert_kind_of(URI::Generic, url)
    assert_equal('http:g', url.to_s)
  end

  def test_join
    assert_equal(URI.parse('http://foo/bar'), URI.join('http://foo/bar'))
    assert_equal(URI.parse('http://foo/bar'), URI.join('http://foo', 'bar'))
    assert_equal(URI.parse('http://foo/bar/'), URI.join('http://foo', 'bar/'))

    assert_equal(URI.parse('http://foo/baz'), URI.join('http://foo', 'bar', 'baz'))
    assert_equal(URI.parse('http://foo/baz'), URI.join('http://foo', 'bar', '/baz'))
    assert_equal(URI.parse('http://foo/baz/'), URI.join('http://foo', 'bar', '/baz/'))
    assert_equal(URI.parse('http://foo/bar/baz'), URI.join('http://foo', 'bar/', 'baz'))
    assert_equal(URI.parse('http://foo/hoge'), URI.join('http://foo', 'bar', 'baz', 'hoge'))

    assert_equal(URI.parse('http://foo/bar/baz'), URI.join('http://foo', 'bar/baz'))
    assert_equal(URI.parse('http://foo/bar/hoge'), URI.join('http://foo', 'bar/baz', 'hoge'))
    assert_equal(URI.parse('http://foo/bar/baz/hoge'), URI.join('http://foo', 'bar/baz/', 'hoge'))
    assert_equal(URI.parse('http://foo/hoge'), URI.join('http://foo', 'bar/baz', '/hoge'))
    assert_equal(URI.parse('http://foo/bar/hoge'), URI.join('http://foo', 'bar/baz', 'hoge'))
    assert_equal(URI.parse('http://foo/bar/baz/hoge'), URI.join('http://foo', 'bar/baz/', 'hoge'))
    assert_equal(URI.parse('http://foo/hoge'), URI.join('http://foo', 'bar/baz', '/hoge'))
  end

  # ruby-dev:16728
  def test_set_component
    uri = URI.parse('http://foo:bar@baz')
    assert_equal('oof', uri.user = 'oof')
    assert_equal('http://oof:bar@baz', uri.to_s)
    assert_equal('rab', uri.password = 'rab')
    assert_equal('http://oof:rab@baz', uri.to_s)
    assert_equal('foo', uri.userinfo = 'foo')
    assert_equal('http://foo:rab@baz', uri.to_s)
    assert_equal(['foo', 'bar'], uri.userinfo = ['foo', 'bar'])
    assert_equal('http://foo:bar@baz', uri.to_s)
    assert_equal(['foo'], uri.userinfo = ['foo'])
    assert_equal('http://foo:bar@baz', uri.to_s)
    assert_equal('zab', uri.host = 'zab')
    assert_equal('http://foo:bar@zab', uri.to_s)
    assert_equal(8080, uri.port = 8080)
    assert_equal('http://foo:bar@zab:8080', uri.to_s)
    assert_equal('/', uri.path = '/')
    assert_equal('http://foo:bar@zab:8080/', uri.to_s)
    assert_equal('a=1', uri.query = 'a=1')
    assert_equal('http://foo:bar@zab:8080/?a=1', uri.to_s)
    assert_equal('b123', uri.fragment = 'b123')
    assert_equal('http://foo:bar@zab:8080/?a=1#b123', uri.to_s)

    uri = URI.parse('http://example.com')
    assert_raise(URI::InvalidURIError) { uri.password = 'bar' }
    assert_raise(URI::InvalidComponentError) { uri.query = "foo\nbar" }
    uri.userinfo = 'foo:bar'
    assert_equal('http://foo:bar@example.com', uri.to_s)
    assert_raise(URI::InvalidURIError) { uri.registry = 'bar' }
    assert_raise(URI::InvalidURIError) { uri.opaque = 'bar' }

    uri = URI.parse('mailto:foo@example.com')
    assert_raise(URI::InvalidURIError) { uri.user = 'bar' }
    assert_raise(URI::InvalidURIError) { uri.password = 'bar' }
    assert_raise(URI::InvalidURIError) { uri.userinfo = ['bar', 'baz'] }
    assert_raise(URI::InvalidURIError) { uri.host = 'bar' }
    assert_raise(URI::InvalidURIError) { uri.port = 'bar' }
    assert_raise(URI::InvalidURIError) { uri.path = 'bar' }
    assert_raise(URI::InvalidURIError) { uri.query = 'bar' }
  end

  def test_ipv6
    assert_equal("[::1]", URI("http://[::1]/bar/baz").host)
    assert_equal("::1", URI("http://[::1]/bar/baz").hostname)

    u = URI("http://foo/bar")
    assert_equal("http://foo/bar", u.to_s)
    u.hostname = "::1"
    assert_equal("http://[::1]/bar", u.to_s)
  end
end
