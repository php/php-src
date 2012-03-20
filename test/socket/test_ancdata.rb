require 'test/unit'
require 'socket'

class TestSocketAncData < Test::Unit::TestCase
  def test_int
    ancdata = Socket::AncillaryData.int(0, 0, 0, 123)
    assert_equal(123, ancdata.int)
    assert_equal([123].pack("i"), ancdata.data)
  end

  def test_ip_pktinfo
    addr = Addrinfo.ip("127.0.0.1")
    ifindex = 0
    spec_dst = Addrinfo.ip("127.0.0.2")
    begin
      ancdata = Socket::AncillaryData.ip_pktinfo(addr, ifindex, spec_dst)
    rescue NotImplementedError
      return
    end
    assert_equal(Socket::AF_INET, ancdata.family)
    assert_equal(Socket::IPPROTO_IP, ancdata.level)
    assert_equal(Socket::IP_PKTINFO, ancdata.type)
    assert_equal(addr.ip_address, ancdata.ip_pktinfo[0].ip_address)
    assert_equal(ifindex, ancdata.ip_pktinfo[1])
    assert_equal(spec_dst.ip_address, ancdata.ip_pktinfo[2].ip_address)
    assert(ancdata.cmsg_is?(:IP, :PKTINFO))
    assert(ancdata.cmsg_is?("IP", "PKTINFO"))
    assert(ancdata.cmsg_is?(Socket::IPPROTO_IP, Socket::IP_PKTINFO))
    if defined? Socket::IPV6_PKTINFO
      assert(!ancdata.cmsg_is?(:IPV6, :PKTINFO))
    end
    ancdata2 = Socket::AncillaryData.ip_pktinfo(addr, ifindex)
    assert_equal(addr.ip_address, ancdata2.ip_pktinfo[2].ip_address)
  end

  def test_ipv6_pktinfo
    addr = Addrinfo.ip("::1")
    ifindex = 0
    begin
      ancdata = Socket::AncillaryData.ipv6_pktinfo(addr, ifindex)
    rescue NotImplementedError
      return
    end
    assert_equal(Socket::AF_INET6, ancdata.family)
    assert_equal(Socket::IPPROTO_IPV6, ancdata.level)
    assert_equal(Socket::IPV6_PKTINFO, ancdata.type)
    assert_equal(addr.ip_address, ancdata.ipv6_pktinfo[0].ip_address)
    assert_equal(ifindex, ancdata.ipv6_pktinfo[1])
    assert_equal(addr.ip_address, ancdata.ipv6_pktinfo_addr.ip_address)
    assert_equal(ifindex, ancdata.ipv6_pktinfo_ifindex)
    assert(ancdata.cmsg_is?(:IPV6, :PKTINFO))
    assert(ancdata.cmsg_is?("IPV6", "PKTINFO"))
    assert(ancdata.cmsg_is?(Socket::IPPROTO_IPV6, Socket::IPV6_PKTINFO))
    if defined? Socket::IP_PKTINFO
      assert(!ancdata.cmsg_is?(:IP, :PKTINFO))
    end
  end

  if defined?(Socket::SCM_RIGHTS) && defined?(Socket::SCM_TIMESTAMP)
    def test_unix_rights
      assert_raise(TypeError) {
        Socket::AncillaryData.int(:UNIX, :SOL_SOCKET, :TIMESTAMP, 1).unix_rights
      }
    end
  end
end if defined? Socket::AncillaryData
