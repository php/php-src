#
# ipaddr.rb - A class to manipulate an IP address
#
# Copyright (c) 2002 Hajimu UMEMOTO <ume@mahoroba.org>.
# Copyright (c) 2007 Akinori MUSHA <knu@iDaemons.org>.
# All rights reserved.
#
# You can redistribute and/or modify it under the same terms as Ruby.
#
# $Id$
#
# Contact:
#   - Akinori MUSHA <knu@iDaemons.org> (current maintainer)
#
# TODO:
#   - scope_id support
#
require 'socket'

unless Socket.const_defined? "AF_INET6"
  class Socket < BasicSocket
    # IPv6 protocol family
    AF_INET6 = Object.new
  end

  class << IPSocket
    # Returns +true+ if +addr+ is a valid IPv4 address.
    def valid_v4?(addr)
      if /\A(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})\Z/ =~ addr
        return $~.captures.all? {|i| i.to_i < 256}
      end
      return false
    end

    # Returns +true+ if +addr+ is a valid IPv6 address.
    def valid_v6?(addr)
      # IPv6 (normal)
      return true if /\A[\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*\Z/ =~ addr
      return true if /\A[\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*::([\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*)?\Z/ =~ addr
      return true if /\A::([\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*)?\Z/ =~ addr
      # IPv6 (IPv4 compat)
      return true if /\A[\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*:/ =~ addr && valid_v4?($')
      return true if /\A[\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*::([\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*:)?/ =~ addr && valid_v4?($')
      return true if /\A::([\dA-Fa-f]{1,4}(:[\dA-Fa-f]{1,4})*:)?/ =~ addr && valid_v4?($')

      false
    end

    # Returns +true+ if +addr+ is either a valid IPv4 or IPv6 address.
    def valid?(addr)
      valid_v4?(addr) || valid_v6?(addr)
    end

    alias getaddress_orig getaddress

    # Returns a +String+ based representation of a valid DNS hostname,
    # IPv4 or IPv6 address.
    # 
    #   IPSocket.getaddress 'localhost'         #=> "::1"
    #   IPSocket.getaddress 'broadcasthost'     #=> "255.255.255.255"
    #   IPSocket.getaddress 'www.ruby-lang.org' #=> "221.186.184.68"
    #   IPSocket.getaddress 'www.ccc.de'        #=> "2a00:1328:e102:ccc0::122"
    def getaddress(s)
      if valid?(s)
        s
      elsif /\A[-A-Za-z\d.]+\Z/ =~ s
        getaddress_orig(s)
      else
        raise ArgumentError, "invalid address"
      end
    end
  end
end

# IPAddr provides a set of methods to manipulate an IP address.  Both IPv4 and
# IPv6 are supported.
#
# == Example
#
#   require 'ipaddr'
#
#   ipaddr1 = IPAddr.new "3ffe:505:2::1"
#
#   p ipaddr1                   #=> #<IPAddr: IPv6:3ffe:0505:0002:0000:0000:0000:0000:0001/ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff>
#
#   p ipaddr1.to_s              #=> "3ffe:505:2::1"
#
#   ipaddr2 = ipaddr1.mask(48)  #=> #<IPAddr: IPv6:3ffe:0505:0002:0000:0000:0000:0000:0000/ffff:ffff:ffff:0000:0000:0000:0000:0000>
#
#   p ipaddr2.to_s              #=> "3ffe:505:2::"
#
#   ipaddr3 = IPAddr.new "192.168.2.0/24"
#
#   p ipaddr3                   #=> #<IPAddr: IPv4:192.168.2.0/255.255.255.0>

class IPAddr

  # 32 bit mask for IPv4
  IN4MASK = 0xffffffff
  # 128 bit mask for IPv4
  IN6MASK = 0xffffffffffffffffffffffffffffffff
  # Formatstring for IPv6
  IN6FORMAT = (["%.4x"] * 8).join(':')

  # Returns the address family of this IP address.
  attr_reader :family

  # Creates a new ipaddr containing the given network byte ordered
  # string form of an IP address.
  def IPAddr::new_ntoh(addr)
    return IPAddr.new(IPAddr::ntop(addr))
  end

  # Convert a network byte ordered string form of an IP address into
  # human readable form.
  def IPAddr::ntop(addr)
    case addr.size
    when 4
      s = addr.unpack('C4').join('.')
    when 16
      s = IN6FORMAT % addr.unpack('n8')
    else
      raise ArgumentError, "unsupported address family"
    end
    return s
  end

  # Returns a new ipaddr built by bitwise AND.
  def &(other)
    return self.clone.set(@addr & coerce_other(other).to_i)
  end

  # Returns a new ipaddr built by bitwise OR.
  def |(other)
    return self.clone.set(@addr | coerce_other(other).to_i)
  end

  # Returns a new ipaddr built by bitwise right-shift.
  def >>(num)
    return self.clone.set(@addr >> num)
  end

  # Returns a new ipaddr built by bitwise left shift.
  def <<(num)
    return self.clone.set(addr_mask(@addr << num))
  end

  # Returns a new ipaddr built by bitwise negation.
  def ~
    return self.clone.set(addr_mask(~@addr))
  end

  # Returns true if two ipaddrs are equal.
  def ==(other)
    other = coerce_other(other)
    return @family == other.family && @addr == other.to_i
  end

  # Returns a new ipaddr built by masking IP address with the given
  # prefixlen/netmask. (e.g. 8, 64, "255.255.255.0", etc.)
  def mask(prefixlen)
    return self.clone.mask!(prefixlen)
  end

  # Returns true if the given ipaddr is in the range.
  #
  # e.g.:
  #   require 'ipaddr'
  #   net1 = IPAddr.new("192.168.2.0/24")
  #   net2 = IPAddr.new("192.168.2.100")
  #   net3 = IPAddr.new("192.168.3.0")
  #   p net1.include?(net2)     #=> true
  #   p net1.include?(net3)     #=> false
  def include?(other)
    other = coerce_other(other)
    if ipv4_mapped?
      if (@mask_addr >> 32) != 0xffffffffffffffffffffffff
        return false
      end
      mask_addr = (@mask_addr & IN4MASK)
      addr = (@addr & IN4MASK)
      family = Socket::AF_INET
    else
      mask_addr = @mask_addr
      addr = @addr
      family = @family
    end
    if other.ipv4_mapped?
      other_addr = (other.to_i & IN4MASK)
      other_family = Socket::AF_INET
    else
      other_addr = other.to_i
      other_family = other.family
    end

    if family != other_family
      return false
    end
    return ((addr & mask_addr) == (other_addr & mask_addr))
  end
  alias === include?

  # Returns the integer representation of the ipaddr.
  def to_i
    return @addr
  end

  # Returns a string containing the IP address representation.
  def to_s
    str = to_string
    return str if ipv4?

    str.gsub!(/\b0{1,3}([\da-f]+)\b/i, '\1')
    loop do
      break if str.sub!(/\A0:0:0:0:0:0:0:0\Z/, '::')
      break if str.sub!(/\b0:0:0:0:0:0:0\b/, ':')
      break if str.sub!(/\b0:0:0:0:0:0\b/, ':')
      break if str.sub!(/\b0:0:0:0:0\b/, ':')
      break if str.sub!(/\b0:0:0:0\b/, ':')
      break if str.sub!(/\b0:0:0\b/, ':')
      break if str.sub!(/\b0:0\b/, ':')
      break
    end
    str.sub!(/:{3,}/, '::')

    if /\A::(ffff:)?([\da-f]{1,4}):([\da-f]{1,4})\Z/i =~ str
      str = sprintf('::%s%d.%d.%d.%d', $1, $2.hex / 256, $2.hex % 256, $3.hex / 256, $3.hex % 256)
    end

    str
  end

  # Returns a string containing the IP address representation in
  # canonical form.
  def to_string
    return _to_string(@addr)
  end

  # Returns a network byte ordered string form of the IP address.
  def hton
    case @family
    when Socket::AF_INET
      return [@addr].pack('N')
    when Socket::AF_INET6
      return (0..7).map { |i|
        (@addr >> (112 - 16 * i)) & 0xffff
      }.pack('n8')
    else
      raise "unsupported address family"
    end
  end

  # Returns true if the ipaddr is an IPv4 address.
  def ipv4?
    return @family == Socket::AF_INET
  end

  # Returns true if the ipaddr is an IPv6 address.
  def ipv6?
    return @family == Socket::AF_INET6
  end

  # Returns true if the ipaddr is an IPv4-mapped IPv6 address.
  def ipv4_mapped?
    return ipv6? && (@addr >> 32) == 0xffff
  end

  # Returns true if the ipaddr is an IPv4-compatible IPv6 address.
  def ipv4_compat?
    if !ipv6? || (@addr >> 32) != 0
      return false
    end
    a = (@addr & IN4MASK)
    return a != 0 && a != 1
  end

  # Returns a new ipaddr built by converting the native IPv4 address
  # into an IPv4-mapped IPv6 address.
  def ipv4_mapped
    if !ipv4?
      raise ArgumentError, "not an IPv4 address"
    end
    return self.clone.set(@addr | 0xffff00000000, Socket::AF_INET6)
  end

  # Returns a new ipaddr built by converting the native IPv4 address
  # into an IPv4-compatible IPv6 address.
  def ipv4_compat
    if !ipv4?
      raise ArgumentError, "not an IPv4 address"
    end
    return self.clone.set(@addr, Socket::AF_INET6)
  end

  # Returns a new ipaddr built by converting the IPv6 address into a
  # native IPv4 address.  If the IP address is not an IPv4-mapped or
  # IPv4-compatible IPv6 address, returns self.
  def native
    if !ipv4_mapped? && !ipv4_compat?
      return self
    end
    return self.clone.set(@addr & IN4MASK, Socket::AF_INET)
  end

  # Returns a string for DNS reverse lookup.  It returns a string in
  # RFC3172 form for an IPv6 address.
  def reverse
    case @family
    when Socket::AF_INET
      return _reverse + ".in-addr.arpa"
    when Socket::AF_INET6
      return ip6_arpa
    else
      raise "unsupported address family"
    end
  end

  # Returns a string for DNS reverse lookup compatible with RFC3172.
  def ip6_arpa
    if !ipv6?
      raise ArgumentError, "not an IPv6 address"
    end
    return _reverse + ".ip6.arpa"
  end

  # Returns a string for DNS reverse lookup compatible with RFC1886.
  def ip6_int
    if !ipv6?
      raise ArgumentError, "not an IPv6 address"
    end
    return _reverse + ".ip6.int"
  end

  # Returns the successor to the ipaddr.
  def succ
    return self.clone.set(@addr + 1, @family)
  end

  # Compares the ipaddr with another.
  def <=>(other)
    other = coerce_other(other)

    return nil if other.family != @family

    return @addr <=> other.to_i
  end
  include Comparable

  # Checks equality used by Hash.
  def eql?(other)
    return self.class == other.class && self.hash == other.hash && self == other
  end

  # Returns a hash value used by Hash, Set, and Array classes
  def hash
    return ([@addr, @mask_addr].hash << 1) | (ipv4? ? 0 : 1)
  end

  # Creates a Range object for the network address.
  def to_range
    begin_addr = (@addr & @mask_addr)

    case @family
    when Socket::AF_INET
      end_addr = (@addr | (IN4MASK ^ @mask_addr))
    when Socket::AF_INET6
      end_addr = (@addr | (IN6MASK ^ @mask_addr))
    else
      raise "unsupported address family"
    end

    return clone.set(begin_addr, @family)..clone.set(end_addr, @family)
  end

  # Returns a string containing a human-readable representation of the
  # ipaddr. ("#<IPAddr: family:address/mask>")
  def inspect
    case @family
    when Socket::AF_INET
      af = "IPv4"
    when Socket::AF_INET6
      af = "IPv6"
    else
      raise "unsupported address family"
    end
    return sprintf("#<%s: %s:%s/%s>", self.class.name,
                   af, _to_string(@addr), _to_string(@mask_addr))
  end

  protected

  # Set +@addr+, the internal stored ip address, to given +addr+. The
  # parameter +addr+ is validated using the first +family+ member,
  # which is +Socket::AF_INET+ or +Socket::AF_INET6+.
  def set(addr, *family)
    case family[0] ? family[0] : @family
    when Socket::AF_INET
      if addr < 0 || addr > IN4MASK
        raise ArgumentError, "invalid address"
      end
    when Socket::AF_INET6
      if addr < 0 || addr > IN6MASK
        raise ArgumentError, "invalid address"
      end
    else
      raise ArgumentError, "unsupported address family"
    end
    @addr = addr
    if family[0]
      @family = family[0]
    end
    return self
  end

  # Set current netmask to given mask. 
  def mask!(mask)
    if mask.kind_of?(String)
      if mask =~ /^\d+$/
        prefixlen = mask.to_i
      else
        m = IPAddr.new(mask)
        if m.family != @family
          raise ArgumentError, "address family is not same"
        end
        @mask_addr = m.to_i
        @addr &= @mask_addr
        return self
      end
    else
      prefixlen = mask
    end
    case @family
    when Socket::AF_INET
      if prefixlen < 0 || prefixlen > 32
        raise ArgumentError, "invalid length"
      end
      masklen = 32 - prefixlen
      @mask_addr = ((IN4MASK >> masklen) << masklen)
    when Socket::AF_INET6
      if prefixlen < 0 || prefixlen > 128
        raise ArgumentError, "invalid length"
      end
      masklen = 128 - prefixlen
      @mask_addr = ((IN6MASK >> masklen) << masklen)
    else
      raise "unsupported address family"
    end
    @addr = ((@addr >> masklen) << masklen)
    return self
  end

  private

  # Creates a new ipaddr object either from a human readable IP
  # address representation in string, or from a packed in_addr value
  # followed by an address family.
  #
  # In the former case, the following are the valid formats that will
  # be recognized: "address", "address/prefixlen" and "address/mask",
  # where IPv6 address may be enclosed in square brackets (`[' and
  # `]').  If a prefixlen or a mask is specified, it returns a masked
  # IP address.  Although the address family is determined
  # automatically from a specified string, you can specify one
  # explicitly by the optional second argument.
  #
  # Otherwise an IP address is generated from a packed in_addr value
  # and an address family.
  #
  # The IPAddr class defines many methods and operators, and some of
  # those, such as &, |, include? and ==, accept a string, or a packed
  # in_addr value instead of an IPAddr object.
  def initialize(addr = '::', family = Socket::AF_UNSPEC)
    if !addr.kind_of?(String)
      case family
      when Socket::AF_INET, Socket::AF_INET6
        set(addr.to_i, family)
        @mask_addr = (family == Socket::AF_INET) ? IN4MASK : IN6MASK
        return
      when Socket::AF_UNSPEC
        raise ArgumentError, "address family must be specified"
      else
        raise ArgumentError, "unsupported address family: #{family}"
      end
    end
    prefix, prefixlen = addr.split('/')
    if prefix =~ /^\[(.*)\]$/i
      prefix = $1
      family = Socket::AF_INET6
    end
    # It seems AI_NUMERICHOST doesn't do the job.
    #Socket.getaddrinfo(left, nil, Socket::AF_INET6, Socket::SOCK_STREAM, nil,
    #                  Socket::AI_NUMERICHOST)
    begin
      IPSocket.getaddress(prefix)               # test if address is valid
    rescue
      raise ArgumentError, "invalid address"
    end
    @addr = @family = nil
    if family == Socket::AF_UNSPEC || family == Socket::AF_INET
      @addr = in_addr(prefix)
      if @addr
        @family = Socket::AF_INET
      end
    end
    if !@addr && (family == Socket::AF_UNSPEC || family == Socket::AF_INET6)
      @addr = in6_addr(prefix)
      @family = Socket::AF_INET6
    end
    if family != Socket::AF_UNSPEC && @family != family
      raise ArgumentError, "address family mismatch"
    end
    if prefixlen
      mask!(prefixlen)
    else
      @mask_addr = (@family == Socket::AF_INET) ? IN4MASK : IN6MASK
    end
  end

  def coerce_other(other)
    case other
    when IPAddr
      other
    when String
      self.class.new(other)
    else
      self.class.new(other, @family)
    end
  end

  def in_addr(addr)
    if addr =~ /^\d+\.\d+\.\d+\.\d+$/
      return addr.split('.').inject(0) { |i, s|
        i << 8 | s.to_i
      }
    end
    return nil
  end

  def in6_addr(left)
    case left
    when /^::ffff:(\d+\.\d+\.\d+\.\d+)$/i
      return in_addr($1) + 0xffff00000000
    when /^::(\d+\.\d+\.\d+\.\d+)$/i
      return in_addr($1)
    when /[^0-9a-f:]/i
      raise ArgumentError, "invalid address"
    when /^(.*)::(.*)$/
      left, right = $1, $2
    else
      right = ''
    end
    l = left.split(':')
    r = right.split(':')
    rest = 8 - l.size - r.size
    if rest < 0
      return nil
    end
    return (l + Array.new(rest, '0') + r).inject(0) { |i, s|
      i << 16 | s.hex
    }
  end

  def addr_mask(addr)
    case @family
    when Socket::AF_INET
      return addr & IN4MASK
    when Socket::AF_INET6
      return addr & IN6MASK
    else
      raise "unsupported address family"
    end
  end

  def _reverse
    case @family
    when Socket::AF_INET
      return (0..3).map { |i|
        (@addr >> (8 * i)) & 0xff
      }.join('.')
    when Socket::AF_INET6
      return ("%.32x" % @addr).reverse!.gsub!(/.(?!$)/, '\&.')
    else
      raise "unsupported address family"
    end
  end

  def _to_string(addr)
    case @family
    when Socket::AF_INET
      return (0..3).map { |i|
        (addr >> (24 - 8 * i)) & 0xff
      }.join('.')
    when Socket::AF_INET6
      return (("%.32x" % addr).gsub!(/.{4}(?!$)/, '\&:'))
    else
      raise "unsupported address family"
    end
  end

end

if $0 == __FILE__
  eval DATA.read, nil, $0, __LINE__+4
end

__END__

require 'test/unit'

class TC_IPAddr < Test::Unit::TestCase
  def test_s_new
    assert_nothing_raised {
      IPAddr.new("3FFE:505:ffff::/48")
      IPAddr.new("0:0:0:1::")
      IPAddr.new("2001:200:300::/48")
    }

    a = IPAddr.new
    assert_equal("::", a.to_s)
    assert_equal("0000:0000:0000:0000:0000:0000:0000:0000", a.to_string)
    assert_equal(Socket::AF_INET6, a.family)

    a = IPAddr.new("0123:4567:89ab:cdef:0ABC:DEF0:1234:5678")
    assert_equal("123:4567:89ab:cdef:abc:def0:1234:5678", a.to_s)
    assert_equal("0123:4567:89ab:cdef:0abc:def0:1234:5678", a.to_string)
    assert_equal(Socket::AF_INET6, a.family)

    a = IPAddr.new("3ffe:505:2::/48")
    assert_equal("3ffe:505:2::", a.to_s)
    assert_equal("3ffe:0505:0002:0000:0000:0000:0000:0000", a.to_string)
    assert_equal(Socket::AF_INET6, a.family)
    assert_equal(false, a.ipv4?)
    assert_equal(true, a.ipv6?)
    assert_equal("#<IPAddr: IPv6:3ffe:0505:0002:0000:0000:0000:0000:0000/ffff:ffff:ffff:0000:0000:0000:0000:0000>", a.inspect)

    a = IPAddr.new("3ffe:505:2::/ffff:ffff:ffff::")
    assert_equal("3ffe:505:2::", a.to_s)
    assert_equal("3ffe:0505:0002:0000:0000:0000:0000:0000", a.to_string)
    assert_equal(Socket::AF_INET6, a.family)

    a = IPAddr.new("0.0.0.0")
    assert_equal("0.0.0.0", a.to_s)
    assert_equal("0.0.0.0", a.to_string)
    assert_equal(Socket::AF_INET, a.family)

    a = IPAddr.new("192.168.1.2")
    assert_equal("192.168.1.2", a.to_s)
    assert_equal("192.168.1.2", a.to_string)
    assert_equal(Socket::AF_INET, a.family)
    assert_equal(true, a.ipv4?)
    assert_equal(false, a.ipv6?)

    a = IPAddr.new("192.168.1.2/24")
    assert_equal("192.168.1.0", a.to_s)
    assert_equal("192.168.1.0", a.to_string)
    assert_equal(Socket::AF_INET, a.family)
    assert_equal("#<IPAddr: IPv4:192.168.1.0/255.255.255.0>", a.inspect)

    a = IPAddr.new("192.168.1.2/255.255.255.0")
    assert_equal("192.168.1.0", a.to_s)
    assert_equal("192.168.1.0", a.to_string)
    assert_equal(Socket::AF_INET, a.family)

    assert_equal("0:0:0:1::", IPAddr.new("0:0:0:1::").to_s)
    assert_equal("2001:200:300::", IPAddr.new("2001:200:300::/48").to_s)

    assert_equal("2001:200:300::", IPAddr.new("[2001:200:300::]/48").to_s)

    [
      ["fe80::1%fxp0"],
      ["::1/255.255.255.0"],
      ["::1:192.168.1.2/120"],
      [IPAddr.new("::1").to_i],
      ["::ffff:192.168.1.2/120", Socket::AF_INET],
      ["[192.168.1.2]/120"],
    ].each { |args|
      assert_raises(ArgumentError) {
        IPAddr.new(*args)
      }
    }
  end

  def test_s_new_ntoh
    addr = ''
    IPAddr.new("1234:5678:9abc:def0:1234:5678:9abc:def0").hton.each_byte { |c|
      addr += sprintf("%02x", c)
    }
    assert_equal("123456789abcdef0123456789abcdef0", addr)
    addr = ''
    IPAddr.new("123.45.67.89").hton.each_byte { |c|
      addr += sprintf("%02x", c)
    }
    assert_equal(sprintf("%02x%02x%02x%02x", 123, 45, 67, 89), addr)
    a = IPAddr.new("3ffe:505:2::")
    assert_equal("3ffe:505:2::", IPAddr.new_ntoh(a.hton).to_s)
    a = IPAddr.new("192.168.2.1")
    assert_equal("192.168.2.1", IPAddr.new_ntoh(a.hton).to_s)
  end

  def test_ipv4_compat
    a = IPAddr.new("::192.168.1.2")
    assert_equal("::192.168.1.2", a.to_s)
    assert_equal("0000:0000:0000:0000:0000:0000:c0a8:0102", a.to_string)
    assert_equal(Socket::AF_INET6, a.family)
    assert_equal(true, a.ipv4_compat?)
    b = a.native
    assert_equal("192.168.1.2", b.to_s)
    assert_equal(Socket::AF_INET, b.family)
    assert_equal(false, b.ipv4_compat?)

    a = IPAddr.new("192.168.1.2")
    b = a.ipv4_compat
    assert_equal("::192.168.1.2", b.to_s)
    assert_equal(Socket::AF_INET6, b.family)
  end

  def test_ipv4_mapped
    a = IPAddr.new("::ffff:192.168.1.2")
    assert_equal("::ffff:192.168.1.2", a.to_s)
    assert_equal("0000:0000:0000:0000:0000:ffff:c0a8:0102", a.to_string)
    assert_equal(Socket::AF_INET6, a.family)
    assert_equal(true, a.ipv4_mapped?)
    b = a.native
    assert_equal("192.168.1.2", b.to_s)
    assert_equal(Socket::AF_INET, b.family)
    assert_equal(false, b.ipv4_mapped?)

    a = IPAddr.new("192.168.1.2")
    b = a.ipv4_mapped
    assert_equal("::ffff:192.168.1.2", b.to_s)
    assert_equal(Socket::AF_INET6, b.family)
  end

  def test_reverse
    assert_equal("f.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2.0.0.0.5.0.5.0.e.f.f.3.ip6.arpa", IPAddr.new("3ffe:505:2::f").reverse)
    assert_equal("1.2.168.192.in-addr.arpa", IPAddr.new("192.168.2.1").reverse)
  end

  def test_ip6_arpa
    assert_equal("f.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2.0.0.0.5.0.5.0.e.f.f.3.ip6.arpa", IPAddr.new("3ffe:505:2::f").ip6_arpa)
    assert_raises(ArgumentError) {
      IPAddr.new("192.168.2.1").ip6_arpa
    }
  end

  def test_ip6_int
    assert_equal("f.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.2.0.0.0.5.0.5.0.e.f.f.3.ip6.int", IPAddr.new("3ffe:505:2::f").ip6_int)
    assert_raises(ArgumentError) {
      IPAddr.new("192.168.2.1").ip6_int
    }
  end

  def test_to_s
    assert_equal("3ffe:0505:0002:0000:0000:0000:0000:0001", IPAddr.new("3ffe:505:2::1").to_string)
    assert_equal("3ffe:505:2::1", IPAddr.new("3ffe:505:2::1").to_s)
  end
end

class TC_Operator < Test::Unit::TestCase

  IN6MASK32  = "ffff:ffff::"
  IN6MASK128 = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"

  def setup
    @in6_addr_any = IPAddr.new()
    @a = IPAddr.new("3ffe:505:2::/48")
    @b = IPAddr.new("0:0:0:1::")
    @c = IPAddr.new(IN6MASK32)
  end
  alias set_up setup

  def test_or
    assert_equal("3ffe:505:2:1::", (@a | @b).to_s)
    a = @a
    a |= @b
    assert_equal("3ffe:505:2:1::", a.to_s)
    assert_equal("3ffe:505:2::", @a.to_s)
    assert_equal("3ffe:505:2:1::",
                 (@a | 0x00000000000000010000000000000000).to_s)
  end

  def test_and
    assert_equal("3ffe:505::", (@a & @c).to_s)
    a = @a
    a &= @c
    assert_equal("3ffe:505::", a.to_s)
    assert_equal("3ffe:505:2::", @a.to_s)
    assert_equal("3ffe:505::", (@a & 0xffffffff000000000000000000000000).to_s)
  end

  def test_shift_right
    assert_equal("0:3ffe:505:2::", (@a >> 16).to_s)
    a = @a
    a >>= 16
    assert_equal("0:3ffe:505:2::", a.to_s)
    assert_equal("3ffe:505:2::", @a.to_s)
  end

  def test_shift_left
    assert_equal("505:2::", (@a << 16).to_s)
    a = @a
    a <<= 16
    assert_equal("505:2::", a.to_s)
    assert_equal("3ffe:505:2::", @a.to_s)
  end

  def test_carrot
    a = ~@in6_addr_any
    assert_equal(IN6MASK128, a.to_s)
    assert_equal("::", @in6_addr_any.to_s)
  end

  def test_equal
    assert_equal(true, @a == IPAddr.new("3ffe:505:2::"))
    assert_equal(false, @a == IPAddr.new("3ffe:505:3::"))
    assert_equal(true, @a != IPAddr.new("3ffe:505:3::"))
    assert_equal(false, @a != IPAddr.new("3ffe:505:2::"))
  end

  def test_mask
    a = @a.mask(32)
    assert_equal("3ffe:505::", a.to_s)
    assert_equal("3ffe:505:2::", @a.to_s)
  end

  def test_include?
    assert_equal(true, @a.include?(IPAddr.new("3ffe:505:2::")))
    assert_equal(true, @a.include?(IPAddr.new("3ffe:505:2::1")))
    assert_equal(false, @a.include?(IPAddr.new("3ffe:505:3::")))
    net1 = IPAddr.new("192.168.2.0/24")
    assert_equal(true, net1.include?(IPAddr.new("192.168.2.0")))
    assert_equal(true, net1.include?(IPAddr.new("192.168.2.255")))
    assert_equal(false, net1.include?(IPAddr.new("192.168.3.0")))
    # test with integer parameter
    int = (192 << 24) + (168 << 16) + (2 << 8) + 13

    assert_equal(true, net1.include?(int))
    assert_equal(false, net1.include?(int+255))

  end

  def test_hash
    a1 = IPAddr.new('192.168.2.0')
    a2 = IPAddr.new('192.168.2.0')
    a3 = IPAddr.new('3ffe:505:2::1')
    a4 = IPAddr.new('3ffe:505:2::1')
    a5 = IPAddr.new('127.0.0.1')
    a6 = IPAddr.new('::1')
    a7 = IPAddr.new('192.168.2.0/25')
    a8 = IPAddr.new('192.168.2.0/25')

    h = { a1 => 'ipv4', a2 => 'ipv4', a3 => 'ipv6', a4 => 'ipv6', a5 => 'ipv4', a6 => 'ipv6', a7 => 'ipv4', a8 => 'ipv4'}
    assert_equal(5, h.size)
    assert_equal('ipv4', h[a1])
    assert_equal('ipv4', h[a2])
    assert_equal('ipv6', h[a3])
    assert_equal('ipv6', h[a4])

    require 'set'
    s = Set[a1, a2, a3, a4, a5, a6, a7, a8]
    assert_equal(5, s.size)
    assert_equal(true, s.include?(a1))
    assert_equal(true, s.include?(a2))
    assert_equal(true, s.include?(a3))
    assert_equal(true, s.include?(a4))
    assert_equal(true, s.include?(a5))
    assert_equal(true, s.include?(a6))
  end
end
