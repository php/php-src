require_relative 'utils'

if defined?(OpenSSL)

class OpenSSL::TestX509Extension < Test::Unit::TestCase
  def setup
    @basic_constraints_value = OpenSSL::ASN1::Sequence([
      OpenSSL::ASN1::Boolean(true),   # CA
      OpenSSL::ASN1::Integer(2)       # pathlen
    ])
    @basic_constraints = OpenSSL::ASN1::Sequence([
      OpenSSL::ASN1::ObjectId("basicConstraints"),
      OpenSSL::ASN1::Boolean(true),
      OpenSSL::ASN1::OctetString(@basic_constraints_value.to_der),
    ])
  end

  def teardown
  end

  def test_new
    ext = OpenSSL::X509::Extension.new(@basic_constraints.to_der)
    assert_equal("basicConstraints", ext.oid)
    assert_equal(true, ext.critical?)
    assert_equal("CA:TRUE, pathlen:2", ext.value)

    ext = OpenSSL::X509::Extension.new("2.5.29.19",
                                       @basic_constraints_value.to_der, true)
    assert_equal(@basic_constraints.to_der, ext.to_der)
  end

  def test_create_by_factory
    ef = OpenSSL::X509::ExtensionFactory.new

    bc = ef.create_extension("basicConstraints", "critical, CA:TRUE, pathlen:2")
    assert_equal(@basic_constraints.to_der, bc.to_der)

    bc = ef.create_extension("basicConstraints", "CA:TRUE, pathlen:2", true)
    assert_equal(@basic_constraints.to_der, bc.to_der)

    begin
      ef.config = OpenSSL::Config.parse(<<-_end_of_cnf_)
      [crlDistPts]
      URI.1 = http://www.example.com/crl
      URI.2 = ldap://ldap.example.com/cn=ca?certificateRevocationList;binary
      _end_of_cnf_
    rescue NotImplementedError
      return
    end

    cdp = ef.create_extension("crlDistributionPoints", "@crlDistPts")
    assert_equal(false, cdp.critical?)
    assert_equal("crlDistributionPoints", cdp.oid)
    assert_match(%{URI:http://www\.example\.com/crl}, cdp.value)
    assert_match(
      %r{URI:ldap://ldap\.example\.com/cn=ca\?certificateRevocationList;binary},
      cdp.value)

    cdp = ef.create_extension("crlDistributionPoints", "critical, @crlDistPts")
    assert_equal(true, cdp.critical?)
    assert_equal("crlDistributionPoints", cdp.oid)
    assert_match(%{URI:http://www.example.com/crl}, cdp.value)
    assert_match(
      %r{URI:ldap://ldap.example.com/cn=ca\?certificateRevocationList;binary},
      cdp.value)
  end
end

end
