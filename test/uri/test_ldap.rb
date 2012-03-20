require 'test/unit'
require 'uri/ldap'

module URI


class TestLDAP < Test::Unit::TestCase
  def setup
  end

  def teardown
  end

  def uri_to_ary(uri)
    uri.class.component.collect {|c| uri.send(c)}
  end

  def test_parse
    url = 'ldap://ldap.jaist.ac.jp/o=JAIST,c=JP?sn?base?(sn=ttate*)'
    u = URI.parse(url)
    assert_kind_of(URI::LDAP, u)
    assert_equal(url, u.to_s)
    assert_equal('o=JAIST,c=JP', u.dn)
    assert_equal('sn', u.attributes)
    assert_equal('base', u.scope)
    assert_equal('(sn=ttate*)', u.filter)
    assert_equal(nil, u.extensions)

    u.scope = URI::LDAP::SCOPE_SUB
    u.attributes = 'sn,cn,mail'
    assert_equal('ldap://ldap.jaist.ac.jp/o=JAIST,c=JP?sn,cn,mail?sub?(sn=ttate*)', u.to_s)
    assert_equal('o=JAIST,c=JP', u.dn)
    assert_equal('sn,cn,mail', u.attributes)
    assert_equal('sub', u.scope)
    assert_equal('(sn=ttate*)', u.filter)
    assert_equal(nil, u.extensions)

    # from RFC2255, section 6.
    {
      'ldap:///o=University%20of%20Michigan,c=US' =>
      ['ldap', nil, URI::LDAP::DEFAULT_PORT,
	'o=University%20of%20Michigan,c=US',
	nil, nil, nil, nil],

      'ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US' =>
      ['ldap', 'ldap.itd.umich.edu', URI::LDAP::DEFAULT_PORT,
	'o=University%20of%20Michigan,c=US',
	nil, nil, nil, nil],

      'ldap://ldap.itd.umich.edu/o=University%20of%20Michigan,c=US?postalAddress' =>
      ['ldap', 'ldap.itd.umich.edu', URI::LDAP::DEFAULT_PORT,
	'o=University%20of%20Michigan,c=US',
	'postalAddress', nil, nil, nil],

      'ldap://host.com:6666/o=University%20of%20Michigan,c=US??sub?(cn=Babs%20Jensen)' =>
      ['ldap', 'host.com', 6666,
	'o=University%20of%20Michigan,c=US',
	nil, 'sub', '(cn=Babs%20Jensen)', nil],

      'ldap://ldap.itd.umich.edu/c=GB?objectClass?one' =>
      ['ldap', 'ldap.itd.umich.edu', URI::LDAP::DEFAULT_PORT,
	'c=GB',
	'objectClass', 'one', nil, nil],

      'ldap://ldap.question.com/o=Question%3f,c=US?mail' =>
      ['ldap', 'ldap.question.com', URI::LDAP::DEFAULT_PORT,
	'o=Question%3f,c=US',
	'mail', nil, nil, nil],

      'ldap://ldap.netscape.com/o=Babsco,c=US??(int=%5c00%5c00%5c00%5c04)' =>
      ['ldap', 'ldap.netscape.com', URI::LDAP::DEFAULT_PORT,
	'o=Babsco,c=US',
	nil, '(int=%5c00%5c00%5c00%5c04)', nil, nil],

      'ldap:///??sub??bindname=cn=Manager%2co=Foo' =>
      ['ldap', nil, URI::LDAP::DEFAULT_PORT,
	'',
	nil, 'sub', nil, 'bindname=cn=Manager%2co=Foo'],

      'ldap:///??sub??!bindname=cn=Manager%2co=Foo' =>
      ['ldap', nil, URI::LDAP::DEFAULT_PORT,
	'',
	nil, 'sub', nil, '!bindname=cn=Manager%2co=Foo'],
    }.each do |url2, ary|
      u = URI.parse(url2)
      assert_equal(ary, uri_to_ary(u))
    end
  end

  def test_select
    u = URI.parse('ldap:///??sub??!bindname=cn=Manager%2co=Foo')
    assert_equal(uri_to_ary(u), u.select(*u.component))
    assert_raise(ArgumentError) do
      u.select(:scheme, :host, :not_exist, :port)
    end
  end
end


end
