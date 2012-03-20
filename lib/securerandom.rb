# = Secure random number generator interface.
#
# This library is an interface for secure random number generator which is
# suitable for generating session key in HTTP cookies, etc.
#
# It supports following secure random number generators.
#
# * openssl
# * /dev/urandom
# * Win32
#
# == Example
#
# # random hexadecimal string.
# p SecureRandom.hex(10) #=> "52750b30ffbc7de3b362"
# p SecureRandom.hex(10) #=> "92b15d6c8dc4beb5f559"
# p SecureRandom.hex(11) #=> "6aca1b5c58e4863e6b81b8"
# p SecureRandom.hex(12) #=> "94b2fff3e7fd9b9c391a2306"
# p SecureRandom.hex(13) #=> "39b290146bea6ce975c37cfc23"
# ...
#
# # random base64 string.
# p SecureRandom.base64(10) #=> "EcmTPZwWRAozdA=="
# p SecureRandom.base64(10) #=> "9b0nsevdwNuM/w=="
# p SecureRandom.base64(10) #=> "KO1nIU+p9DKxGg=="
# p SecureRandom.base64(11) #=> "l7XEiFja+8EKEtY="
# p SecureRandom.base64(12) #=> "7kJSM/MzBJI+75j8"
# p SecureRandom.base64(13) #=> "vKLJ0tXBHqQOuIcSIg=="
# ...
#
# # random binary string.
# p SecureRandom.random_bytes(10) #=> "\016\t{\370g\310pbr\301"
# p SecureRandom.random_bytes(10) #=> "\323U\030TO\234\357\020\a\337"
# ...

begin
  require 'openssl'
rescue LoadError
end

module SecureRandom
  # SecureRandom.random_bytes generates a random binary string.
  #
  # The argument _n_ specifies the length of the result string.
  #
  # If _n_ is not specified, 16 is assumed.
  # It may be larger in future.
  #
  # The result may contain any byte: "\x00" - "\xff".
  #
  #   p SecureRandom.random_bytes #=> "\xD8\\\xE0\xF4\r\xB2\xFC*WM\xFF\x83\x18\xF45\xB6"
  #   p SecureRandom.random_bytes #=> "m\xDC\xFC/\a\x00Uf\xB2\xB2P\xBD\xFF6S\x97"
  #
  # If secure random number generator is not available,
  # NotImplementedError is raised.
  def self.random_bytes(n=nil)
    n ||= 16

    if defined? OpenSSL::Random
      @pid = 0 if !defined?(@pid)
      pid = $$
      if @pid != pid
        now = Time.now
        ary = [now.to_i, now.nsec, @pid, pid]
        OpenSSL::Random.seed(ary.to_s)
        @pid = pid
      end
      return OpenSSL::Random.random_bytes(n)
    end

    if !defined?(@has_urandom) || @has_urandom
      flags = File::RDONLY
      flags |= File::NONBLOCK if defined? File::NONBLOCK
      flags |= File::NOCTTY if defined? File::NOCTTY
      begin
        File.open("/dev/urandom", flags) {|f|
          unless f.stat.chardev?
            raise Errno::ENOENT
          end
          @has_urandom = true
          ret = f.read(n)
          if ret.length != n
            raise NotImplementedError, "Unexpected partial read from random device"
          end
          return ret
        }
      rescue Errno::ENOENT
        @has_urandom = false
      end
    end

    if !defined?(@has_win32)
      begin
        require 'Win32API'

        crypt_acquire_context = Win32API.new("advapi32", "CryptAcquireContext", 'PPPII', 'L')
        @crypt_gen_random = Win32API.new("advapi32", "CryptGenRandom", 'LIP', 'L')

        hProvStr = " " * 4
        prov_rsa_full = 1
        crypt_verifycontext = 0xF0000000

        if crypt_acquire_context.call(hProvStr, nil, nil, prov_rsa_full, crypt_verifycontext) == 0
          raise SystemCallError, "CryptAcquireContext failed: #{lastWin32ErrorMessage}"
        end
        @hProv, = hProvStr.unpack('L')

        @has_win32 = true
      rescue LoadError
        @has_win32 = false
      end
    end
    if @has_win32
      bytes = " ".force_encoding("ASCII-8BIT") * n
      if @crypt_gen_random.call(@hProv, bytes.size, bytes) == 0
        raise SystemCallError, "CryptGenRandom failed: #{lastWin32ErrorMessage}"
      end
      return bytes
    end

    raise NotImplementedError, "No random device"
  end

  # SecureRandom.hex generates a random hex string.
  #
  # The argument _n_ specifies the length of the random length.
  # The length of the result string is twice of _n_.
  #
  # If _n_ is not specified, 16 is assumed.
  # It may be larger in future.
  #
  # The result may contain 0-9 and a-f.
  #
  #   p SecureRandom.hex #=> "eb693ec8252cd630102fd0d0fb7c3485"
  #   p SecureRandom.hex #=> "91dc3bfb4de5b11d029d376634589b61"
  #
  # If secure random number generator is not available,
  # NotImplementedError is raised.
  def self.hex(n=nil)
    random_bytes(n).unpack("H*")[0]
  end

  # SecureRandom.base64 generates a random base64 string.
  #
  # The argument _n_ specifies the length of the random length.
  # The length of the result string is about 4/3 of _n_.
  #
  # If _n_ is not specified, 16 is assumed.
  # It may be larger in future.
  #
  # The result may contain A-Z, a-z, 0-9, "+", "/" and "=".
  #
  #   p SecureRandom.base64 #=> "/2BuBuLf3+WfSKyQbRcc/A=="
  #   p SecureRandom.base64 #=> "6BbW0pxO0YENxn38HMUbcQ=="
  #
  # If secure random number generator is not available,
  # NotImplementedError is raised.
  #
  # See RFC 3548 for the definition of base64.
  def self.base64(n=nil)
    [random_bytes(n)].pack("m*").delete("\n")
  end

  # SecureRandom.urlsafe_base64 generates a random URL-safe base64 string.
  #
  # The argument _n_ specifies the length of the random length.
  # The length of the result string is about 4/3 of _n_.
  #
  # If _n_ is not specified, 16 is assumed.
  # It may be larger in future.
  #
  # The boolean argument _padding_ specifies the padding.
  # If it is false or nil, padding is not generated.
  # Otherwise padding is generated.
  # By default, padding is not generated because "=" may be used as a URL delimiter.
  #
  # The result may contain A-Z, a-z, 0-9, "-" and "_".
  # "=" is also used if _padding_ is true.
  #
  #   p SecureRandom.urlsafe_base64 #=> "b4GOKm4pOYU_-BOXcrUGDg"
  #   p SecureRandom.urlsafe_base64 #=> "UZLdOkzop70Ddx-IJR0ABg"
  #
  #   p SecureRandom.urlsafe_base64(nil, true) #=> "i0XQ-7gglIsHGV2_BNPrdQ=="
  #   p SecureRandom.urlsafe_base64(nil, true) #=> "-M8rLhr7JEpJlqFGUMmOxg=="
  #
  # If secure random number generator is not available,
  # NotImplementedError is raised.
  #
  # See RFC 3548 for the definition of URL-safe base64.
  def self.urlsafe_base64(n=nil, padding=false)
    s = [random_bytes(n)].pack("m*")
    s.delete!("\n")
    s.tr!("+/", "-_")
    s.delete!("=") if !padding
    s
  end

  # SecureRandom.random_number generates a random number.
  #
  # If a positive integer is given as _n_,
  # SecureRandom.random_number returns an integer:
  # 0 <= SecureRandom.random_number(n) < n.
  #
  #   p SecureRandom.random_number(100) #=> 15
  #   p SecureRandom.random_number(100) #=> 88
  #
  # If 0 is given or an argument is not given,
  # SecureRandom.random_number returns a float:
  # 0.0 <= SecureRandom.random_number() < 1.0.
  #
  #   p SecureRandom.random_number #=> 0.596506046187744
  #   p SecureRandom.random_number #=> 0.350621695741409
  #
  def self.random_number(n=0)
    if 0 < n
      hex = n.to_s(16)
      hex = '0' + hex if (hex.length & 1) == 1
      bin = [hex].pack("H*")
      mask = bin[0].ord
      mask |= mask >> 1
      mask |= mask >> 2
      mask |= mask >> 4
      begin
        rnd = SecureRandom.random_bytes(bin.length)
        rnd[0] = (rnd[0].ord & mask).chr
      end until rnd < bin
      rnd.unpack("H*")[0].hex
    else
      # assumption: Float::MANT_DIG <= 64
      i64 = SecureRandom.random_bytes(8).unpack("Q")[0]
      Math.ldexp(i64 >> (64-Float::MANT_DIG), -Float::MANT_DIG)
    end
  end

  # SecureRandom.uuid generates a v4 random UUID (Universally Unique IDentifier).
  #
  #   p SecureRandom.uuid #=> "2d931510-d99f-494a-8c67-87feb05e1594"
  #   p SecureRandom.uuid #=> "bad85eb9-0713-4da7-8d36-07a8e4b00eab"
  #   p SecureRandom.uuid #=> "62936e70-1815-439b-bf89-8492855a7e6b"
  #
  # The version 4 UUID is purely random (except the version).
  # It doesn't contain meaningful information such as MAC address, time, etc.
  #
  # See RFC 4122 for details of UUID.
  #
  def self.uuid
    ary = self.random_bytes(16).unpack("NnnnnN")
    ary[2] = (ary[2] & 0x0fff) | 0x4000
    ary[3] = (ary[3] & 0x3fff) | 0x8000
    "%08x-%04x-%04x-%04x-%04x%08x" % ary
  end

  # Following code is based on David Garamond's GUID library for Ruby.
  def self.lastWin32ErrorMessage # :nodoc:
    get_last_error = Win32API.new("kernel32", "GetLastError", '', 'L')
    format_message = Win32API.new("kernel32", "FormatMessageA", 'LPLLPLPPPPPPPP', 'L')
    format_message_ignore_inserts = 0x00000200
    format_message_from_system    = 0x00001000

    code = get_last_error.call
    msg = "\0" * 1024
    len = format_message.call(format_message_ignore_inserts + format_message_from_system, 0, code, 0, msg, 1024, nil, nil, nil, nil, nil, nil, nil, nil)
    msg[0, len].tr("\r", '').chomp
  end
end
