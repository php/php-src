require 'test/unit'
require 'resolv'
require 'socket'

class TestResolvDNS < Test::Unit::TestCase
  def setup
    @save_do_not_reverse_lookup = BasicSocket.do_not_reverse_lookup
    BasicSocket.do_not_reverse_lookup = true
  end

  def teardown
    BasicSocket.do_not_reverse_lookup = @save_do_not_reverse_lookup
  end

  def with_udp(host, port)
    u = UDPSocket.new
    begin
      u.bind(host, port)
      yield u
    ensure
      u.close
    end
  end

  def test_query_ipv4_address
    begin
      OpenSSL
    rescue LoadError
      skip 'autoload problem. see [ruby-dev:45021][Bug #5786]'
    end if defined?(OpenSSL)

    with_udp('127.0.0.1', 0) {|u|
      _, server_port, _, server_address = u.addr
      begin
        th = Thread.new {
          Resolv::DNS.open(:nameserver_port => [[server_address, server_port]]) {|dns|
            dns.getresources("foo.example.org", Resolv::DNS::Resource::IN::A)
          }
        }
        msg, (af, client_port, _, client_address) = u.recvfrom(4096)
        id, word2, qdcount, ancount, nscount, arcount = msg.unpack("nnnnnn")
        qr =     (word2 & 0x8000) >> 15
        opcode = (word2 & 0x7800) >> 11
        aa =     (word2 & 0x0400) >> 10
        tc =     (word2 & 0x0200) >> 9
        rd =     (word2 & 0x0100) >> 8
        ra =     (word2 & 0x0080) >> 7
        z =      (word2 & 0x0070) >> 4
        rcode =   word2 & 0x000f
        rest = msg[12..-1]
        assert_equal(0, qr) # 0:query 1:response
        assert_equal(0, opcode) # 0:QUERY 1:IQUERY 2:STATUS
        assert_equal(0, aa) # Authoritative Answer
        assert_equal(0, tc) # TrunCation
        assert_equal(1, rd) # Recursion Desired
        assert_equal(0, ra) # Recursion Available
        assert_equal(0, z) # Reserved for future use
        assert_equal(0, rcode) # 0:No-error 1:Format-error 2:Server-failure 3:Name-Error 4:Not-Implemented 5:Refused
        assert_equal(1, qdcount) # number of entries in the question section.
        assert_equal(0, ancount) # number of entries in the answer section.
        assert_equal(0, nscount) # number of entries in the authority records section.
        assert_equal(0, arcount) # number of entries in the additional records section.
        name = [3, "foo", 7, "example", 3, "org", 0].pack("Ca*Ca*Ca*C")
        assert_operator(rest, :start_with?, name)
        rest = rest[name.length..-1]
        assert_equal(4, rest.length)
        qtype, qclass = rest.unpack("nn")
        assert_equal(1, qtype) # A
        assert_equal(1, qtype) # IN
        id = id
        qr = 1
        opcode = opcode
        aa = 0
        tc = 0
        rd = rd
        ra = 1
        z = 0
        rcode = 0
        qdcount = 0
        ancount = 1
        nscount = 0
        arcount = 0
        word2 = (qr << 15) |
                (opcode << 11) |
                (aa << 10) |
                (tc << 9) |
                (rd << 8) |
                (ra << 7) |
                (z << 4) |
                rcode
        msg = [id, word2, qdcount, ancount, nscount, arcount].pack("nnnnnn")
        type = 1
        klass = 1
        ttl = 3600
        rdlength = 4
        rdata = [192,0,2,1].pack("CCCC") # 192.0.2.1 (TEST-NET address) RFC 3330
        rr = [name, type, klass, ttl, rdlength, rdata].pack("a*nnNna*")
        msg << rr
        u.send(msg, 0, client_address, client_port)
        result = th.value
        assert_instance_of(Array, result)
        assert_equal(1, result.length)
        rr = result[0]
        assert_instance_of(Resolv::DNS::Resource::IN::A, rr)
        assert_instance_of(Resolv::IPv4, rr.address)
        assert_equal("192.0.2.1", rr.address.to_s)
        assert_equal(3600, rr.ttl)
      ensure
        th.join
      end
    }
  end

  def test_query_ipv4_address_timeout
    with_udp('127.0.0.1', 0) {|u|
      _, port , _, host = u.addr
      start = nil
      rv = Resolv::DNS.open(:nameserver_port => [[host, port]]) {|dns|
        dns.timeouts = 0.1
        start = Time.now
        dns.getresources("foo.example.org", Resolv::DNS::Resource::IN::A)
      }
      t2 = Time.now
      diff = t2 - start
      assert rv.empty?, "unexpected: #{rv.inspect} (expected empty)"
      assert_operator 0.1, :<=, diff

      rv = Resolv::DNS.open(:nameserver_port => [[host, port]]) {|dns|
        dns.timeouts = [ 0.1, 0.2 ]
        start = Time.now
        dns.getresources("foo.example.org", Resolv::DNS::Resource::IN::A)
      }
      t2 = Time.now
      diff = t2 - start
      assert rv.empty?, "unexpected: #{rv.inspect} (expected empty)"
      assert_operator 0.3, :<=, diff
    }
  end

  def test_no_server
    u = UDPSocket.new
    u.bind("127.0.0.1", 0)
    _, port, _, host = u.addr
    u.close
    # A rase condition here.
    # Another program may use the port.
    # But no way to prevent it.
    Resolv::DNS.open(:nameserver_port => [[host, port]]) {|dns|
      assert_equal([], dns.getresources("test-no-server.example.org", Resolv::DNS::Resource::IN::A))
    }
  end

end
