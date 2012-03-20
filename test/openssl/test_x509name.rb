require_relative 'utils'

if defined?(OpenSSL)

class OpenSSL::TestX509Name < Test::Unit::TestCase
  OpenSSL::ASN1::ObjectId.register(
    "1.2.840.113549.1.9.1", "emailAddress", "emailAddress")
  OpenSSL::ASN1::ObjectId.register(
    "2.5.4.5", "serialNumber", "serialNumber")

  def setup
    @obj_type_tmpl = Hash.new(OpenSSL::ASN1::PRINTABLESTRING)
    @obj_type_tmpl.update(OpenSSL::X509::Name::OBJECT_TYPE_TEMPLATE)
  end

  def teardown
  end

  def test_s_new
    dn = [ ["C", "JP"], ["O", "example"], ["CN", "www.example.jp"] ]
    name = OpenSSL::X509::Name.new(dn)
    ary = name.to_a
    assert_equal("/C=JP/O=example/CN=www.example.jp", name.to_s)
    assert_equal("C", ary[0][0])
    assert_equal("O", ary[1][0])
    assert_equal("CN", ary[2][0])
    assert_equal("JP", ary[0][1])
    assert_equal("example", ary[1][1])
    assert_equal("www.example.jp", ary[2][1])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[2][2])

    dn = [
      ["countryName", "JP"],
      ["organizationName", "example"],
      ["commonName", "www.example.jp"]
    ]
    name = OpenSSL::X509::Name.new(dn)
    ary = name.to_a
    assert_equal("/C=JP/O=example/CN=www.example.jp", name.to_s)
    assert_equal("C", ary[0][0])
    assert_equal("O", ary[1][0])
    assert_equal("CN", ary[2][0])
    assert_equal("JP", ary[0][1])
    assert_equal("example", ary[1][1])
    assert_equal("www.example.jp", ary[2][1])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[2][2])

    name = OpenSSL::X509::Name.new(dn, @obj_type_tmpl)
    ary = name.to_a
    assert_equal("/C=JP/O=example/CN=www.example.jp", name.to_s)
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[2][2])

    dn = [
      ["countryName", "JP", OpenSSL::ASN1::PRINTABLESTRING],
      ["organizationName", "example", OpenSSL::ASN1::PRINTABLESTRING],
      ["commonName", "www.example.jp", OpenSSL::ASN1::PRINTABLESTRING]
    ]
    name = OpenSSL::X509::Name.new(dn)
    ary = name.to_a
    assert_equal("/C=JP/O=example/CN=www.example.jp", name.to_s)
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[2][2])

    dn = [
      ["DC", "org"],
      ["DC", "ruby-lang"],
      ["CN", "GOTOU Yuuzou"],
      ["emailAddress", "gotoyuzo@ruby-lang.org"],
      ["serialNumber", "123"],
    ]
    name = OpenSSL::X509::Name.new(dn)
    ary = name.to_a
    assert_equal("/DC=org/DC=ruby-lang/CN=GOTOU Yuuzou/emailAddress=gotoyuzo@ruby-lang.org/serialNumber=123", name.to_s)
    assert_equal("DC", ary[0][0])
    assert_equal("DC", ary[1][0])
    assert_equal("CN", ary[2][0])
    assert_equal("emailAddress", ary[3][0])
    assert_equal("serialNumber", ary[4][0])
    assert_equal("org", ary[0][1])
    assert_equal("ruby-lang", ary[1][1])
    assert_equal("GOTOU Yuuzou", ary[2][1])
    assert_equal("gotoyuzo@ruby-lang.org", ary[3][1])
    assert_equal("123", ary[4][1])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[2][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[3][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[4][2])

    name_from_der = OpenSSL::X509::Name.new(name.to_der)
    assert_equal(name_from_der.to_s, name.to_s)
    assert_equal(name_from_der.to_a, name.to_a)
    assert_equal(name_from_der.to_der, name.to_der)
  end

  def test_unrecognized_oid
    dn = [ ["1.2.3.4.5.6.7.8.9.7.5.3.1", "Unknown OID 1"],
           ["1.1.2.3.5.8.13.21.34", "Unknown OID 2"],
           ["C", "US"],
           ["postalCode", "60602"],
           ["ST", "Illinois"],
           ["L", "Chicago"],
           #["street", "123 Fake St"],
           ["O", "Some Company LLC"],
           ["CN", "mydomain.com"] ]

    name = OpenSSL::X509::Name.new(dn)
    ary = name.to_a
    #assert_equal("/1.2.3.4.5.6.7.8.9.7.5.3.1=Unknown OID 1/1.1.2.3.5.8.13.21.34=Unknown OID 2/C=US/postalCode=60602/ST=Illinois/L=Chicago/street=123 Fake St/O=Some Company LLC/CN=mydomain.com", name.to_s)
    assert_equal("/1.2.3.4.5.6.7.8.9.7.5.3.1=Unknown OID 1/1.1.2.3.5.8.13.21.34=Unknown OID 2/C=US/postalCode=60602/ST=Illinois/L=Chicago/O=Some Company LLC/CN=mydomain.com", name.to_s)
    assert_equal("1.2.3.4.5.6.7.8.9.7.5.3.1", ary[0][0])
    assert_equal("1.1.2.3.5.8.13.21.34", ary[1][0])
    assert_equal("C", ary[2][0])
    assert_equal("postalCode", ary[3][0])
    assert_equal("ST", ary[4][0])
    assert_equal("L", ary[5][0])
    #assert_equal("street", ary[6][0])
    assert_equal("O", ary[6][0])
    assert_equal("CN", ary[7][0])
    assert_equal("Unknown OID 1", ary[0][1])
    assert_equal("Unknown OID 2", ary[1][1])
    assert_equal("US", ary[2][1])
    assert_equal("60602", ary[3][1])
    assert_equal("Illinois", ary[4][1])
    assert_equal("Chicago", ary[5][1])
    #assert_equal("123 Fake St", ary[6][1])
    assert_equal("Some Company LLC", ary[6][1])
    assert_equal("mydomain.com", ary[7][1])
  end

  def test_unrecognized_oid_parse_encode_equality
    dn = [ ["1.2.3.4.5.6.7.8.9.7.5.3.2", "Unknown OID1"],
           ["1.1.2.3.5.8.13.21.35", "Unknown OID2"],
           ["C", "US"],
           ["postalCode", "60602"],
           ["ST", "Illinois"],
           ["L", "Chicago"],
           #["street", "123 Fake St"],
           ["O", "Some Company LLC"],
           ["CN", "mydomain.com"] ]

    name1 = OpenSSL::X509::Name.new(dn)
    name2 = OpenSSL::X509::Name.parse(name1.to_s)
    assert_equal(name1.to_s, name2.to_s)
    assert_equal(name1.to_a, name2.to_a)
  end

  def test_s_parse
    dn = "/DC=org/DC=ruby-lang/CN=www.ruby-lang.org"
    name = OpenSSL::X509::Name.parse(dn)
    assert_equal(dn, name.to_s)
    ary = name.to_a
    assert_equal("DC", ary[0][0])
    assert_equal("DC", ary[1][0])
    assert_equal("CN", ary[2][0])
    assert_equal("org", ary[0][1])
    assert_equal("ruby-lang", ary[1][1])
    assert_equal("www.ruby-lang.org", ary[2][1])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[2][2])

    dn2 = "DC=org, DC=ruby-lang, CN=www.ruby-lang.org"
    name = OpenSSL::X509::Name.parse(dn2)
    ary = name.to_a
    assert_equal(dn, name.to_s)
    assert_equal("org", ary[0][1])
    assert_equal("ruby-lang", ary[1][1])
    assert_equal("www.ruby-lang.org", ary[2][1])

    name = OpenSSL::X509::Name.parse(dn2, @obj_type_tmpl)
    ary = name.to_a
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[2][2])
  end

  def test_s_parse_rfc2253
    scanner = OpenSSL::X509::Name::RFC2253DN.method(:scan)

    assert_equal([["C", "JP"]], scanner.call("C=JP"))
    assert_equal([
        ["DC", "org"],
        ["DC", "ruby-lang"],
        ["CN", "GOTOU Yuuzou"],
        ["emailAddress", "gotoyuzo@ruby-lang.org"],
      ],
      scanner.call(
        "emailAddress=gotoyuzo@ruby-lang.org,CN=GOTOU Yuuzou,"+
        "DC=ruby-lang,DC=org")
    )

    u8 = OpenSSL::ASN1::UTF8STRING
    assert_equal([
        ["DC", "org"],
        ["DC", "ruby-lang"],
        ["O", ",=+<>#;"],
        ["O", ",=+<>#;"],
        ["OU", ""],
        ["OU", ""],
        ["L", "aaa=\"bbb, ccc\""],
        ["L", "aaa=\"bbb, ccc\""],
        ["CN", "\345\276\214\350\227\244\350\243\225\350\224\265"],
        ["CN", "\345\276\214\350\227\244\350\243\225\350\224\265"],
        ["CN", "\345\276\214\350\227\244\350\243\225\350\224\265"],
        ["CN", "\345\276\214\350\227\244\350\243\225\350\224\265", u8],
        ["2.5.4.3", "GOTOU, Yuuzou"],
        ["2.5.4.3", "GOTOU, Yuuzou"],
        ["2.5.4.3", "GOTOU, Yuuzou"],
        ["2.5.4.3", "GOTOU, Yuuzou"],
        ["CN", "GOTOU \"gotoyuzo\" Yuuzou"],
        ["CN", "GOTOU \"gotoyuzo\" Yuuzou"],
        ["1.2.840.113549.1.9.1", "gotoyuzo@ruby-lang.org"],
        ["emailAddress", "gotoyuzo@ruby-lang.org"],
      ],
      scanner.call(
        "emailAddress=gotoyuzo@ruby-lang.org," +
        "1.2.840.113549.1.9.1=gotoyuzo@ruby-lang.org," +
        'CN=GOTOU \"gotoyuzo\" Yuuzou,' +
        'CN="GOTOU \"gotoyuzo\" Yuuzou",' +
        '2.5.4.3=GOTOU\,\20Yuuzou,' +
        '2.5.4.3=GOTOU\, Yuuzou,' +
        '2.5.4.3="GOTOU, Yuuzou",' +
        '2.5.4.3="GOTOU\, Yuuzou",' +
        "CN=#0C0CE5BE8CE897A4E8A395E894B5," +
        'CN=\E5\BE\8C\E8\97\A4\E8\A3\95\E8\94\B5,' +
        "CN=\"\xE5\xBE\x8C\xE8\x97\xA4\xE8\xA3\x95\xE8\x94\xB5\"," +
        "CN=\xE5\xBE\x8C\xE8\x97\xA4\xE8\xA3\x95\xE8\x94\xB5," +
        'L=aaa\=\"bbb\, ccc\",' +
        'L="aaa=\"bbb, ccc\"",' +
        'OU=,' +
        'OU="",' +
        'O=\,\=\+\<\>\#\;,' +
        'O=",=+<>#;",' +
        "DC=ruby-lang," +
        "DC=org")
    )

    [
      "DC=org+DC=jp",
      "DC=org,DC=ruby-lang+DC=rubyist,DC=www"
    ].each{|dn|
      ex = scanner.call(dn) rescue $!
      dn_r = Regexp.escape(dn)
      assert_match(/^multi-valued RDN is not supported: #{dn_r}/, ex.message)
    }

    [
      ["DC=org,DC=exapmle,CN", "CN"],
      ["DC=org,DC=example,", ""],
      ["DC=org,DC=exapmle,CN=www.example.org;", "CN=www.example.org;"],
      ["DC=org,DC=exapmle,CN=#www.example.org", "CN=#www.example.org"],
      ["DC=org,DC=exapmle,CN=#777777.example.org", "CN=#777777.example.org"],
      ["DC=org,DC=exapmle,CN=\"www.example\".org", "CN=\"www.example\".org"],
      ["DC=org,DC=exapmle,CN=www.\"example.org\"", "CN=www.\"example.org\""],
      ["DC=org,DC=exapmle,CN=www.\"example\".org", "CN=www.\"example\".org"],
    ].each{|dn, msg|
      ex = scanner.call(dn) rescue $!
      assert_match(/^malformed RDN: .*=>#{Regexp.escape(msg)}/, ex.message)
    }

    dn = "CN=www.ruby-lang.org,DC=ruby-lang,DC=org"
    name = OpenSSL::X509::Name.parse_rfc2253(dn)
    assert_equal(dn, name.to_s(OpenSSL::X509::Name::RFC2253))
    ary = name.to_a
    assert_equal("DC", ary[0][0])
    assert_equal("DC", ary[1][0])
    assert_equal("CN", ary[2][0])
    assert_equal("org", ary[0][1])
    assert_equal("ruby-lang", ary[1][1])
    assert_equal("www.ruby-lang.org", ary[2][1])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[2][2])
  end

  def test_add_entry
    dn = [
      ["DC", "org"],
      ["DC", "ruby-lang"],
      ["CN", "GOTOU Yuuzou"],
      ["emailAddress", "gotoyuzo@ruby-lang.org"],
      ["serialNumber", "123"],
    ]
    name = OpenSSL::X509::Name.new
    dn.each{|attr| name.add_entry(*attr) }
    ary = name.to_a
    assert_equal("/DC=org/DC=ruby-lang/CN=GOTOU Yuuzou/emailAddress=gotoyuzo@ruby-lang.org/serialNumber=123", name.to_s)
    assert_equal("DC", ary[0][0])
    assert_equal("DC", ary[1][0])
    assert_equal("CN", ary[2][0])
    assert_equal("emailAddress", ary[3][0])
    assert_equal("serialNumber", ary[4][0])
    assert_equal("org", ary[0][1])
    assert_equal("ruby-lang", ary[1][1])
    assert_equal("GOTOU Yuuzou", ary[2][1])
    assert_equal("gotoyuzo@ruby-lang.org", ary[3][1])
    assert_equal("123", ary[4][1])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[0][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[1][2])
    assert_equal(OpenSSL::ASN1::UTF8STRING, ary[2][2])
    assert_equal(OpenSSL::ASN1::IA5STRING, ary[3][2])
    assert_equal(OpenSSL::ASN1::PRINTABLESTRING, ary[4][2])
  end

  def test_add_entry_street
    return if OpenSSL::OPENSSL_VERSION_NUMBER < 0x009080df # 0.9.8m
    # openssl/crypto/objects/obj_mac.h 1.83
    dn = [
      ["DC", "org"],
      ["DC", "ruby-lang"],
      ["CN", "GOTOU Yuuzou"],
      ["emailAddress", "gotoyuzo@ruby-lang.org"],
      ["serialNumber", "123"],
      ["street", "Namiki"],
    ]
    name = OpenSSL::X509::Name.new
    dn.each{|attr| name.add_entry(*attr) }
    ary = name.to_a
    assert_equal("/DC=org/DC=ruby-lang/CN=GOTOU Yuuzou/emailAddress=gotoyuzo@ruby-lang.org/serialNumber=123/street=Namiki", name.to_s)
    assert_equal("Namiki", ary[5][1])
  end

  def test_equals2
    n1 = OpenSSL::X509::Name.parse 'CN=a'
    n2 = OpenSSL::X509::Name.parse 'CN=a'

    assert_equal n1, n2
  end

  def test_spaceship
    n1 = OpenSSL::X509::Name.parse 'CN=a'
    n2 = OpenSSL::X509::Name.parse 'CN=b'

    assert_equal -1, n1 <=> n2
  end

  def name_hash(name)
    # OpenSSL 1.0.0 uses SHA1 for canonical encoding (not just a der) of
    # X509Name for X509_NAME_hash.
    name.respond_to?(:hash_old) ? name.hash_old : name.hash
  end

  def test_hash
    dn = "/DC=org/DC=ruby-lang/CN=www.ruby-lang.org"
    name = OpenSSL::X509::Name.parse(dn)
    d = Digest::MD5.digest(name.to_der)
    expected = (d[0].ord & 0xff) | (d[1].ord & 0xff) << 8 | (d[2].ord & 0xff) << 16 | (d[3].ord & 0xff) << 24
    assert_equal(expected, name_hash(name))
    #
    dn = "/DC=org/DC=ruby-lang/CN=baz.ruby-lang.org"
    name = OpenSSL::X509::Name.parse(dn)
    d = Digest::MD5.digest(name.to_der)
    expected = (d[0].ord & 0xff) | (d[1].ord & 0xff) << 8 | (d[2].ord & 0xff) << 16 | (d[3].ord & 0xff) << 24
    assert_equal(expected, name_hash(name))
  end
end

end
