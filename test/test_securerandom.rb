require 'test/unit'
require 'securerandom'
require 'tempfile'

# This testcase does NOT aim to test cryptographically strongness and randomness.
class TestSecureRandom < Test::Unit::TestCase
  def setup
    @it = SecureRandom
  end

  def test_s_random_bytes
    assert_equal(16, @it.random_bytes.size)
    assert_equal(Encoding::ASCII_8BIT, @it.random_bytes.encoding)
    65.times do |idx|
      assert_equal(idx, @it.random_bytes(idx).size)
    end
  end

# This test took 2 minutes on my machine.
# And 65536 times loop could not be enough for forcing PID recycle.
if false
  def test_s_random_bytes_is_fork_safe
    begin
      require 'openssl'
    rescue LoadError
      return
    end
    SecureRandom.random_bytes(8)
    pid, v1 = forking_random_bytes
    assert(check_forking_random_bytes(pid, v1), 'Process ID not recycled?')
  end

  def forking_random_bytes
    r, w = IO.pipe
    pid = fork {
      r.close
      w.write SecureRandom.random_bytes(8)
      w.close
    }
    w.close
    v = r.read(8)
    r.close
    Process.waitpid2(pid)
    [pid, v]
  end

  def check_forking_random_bytes(target_pid, target)
    65536.times do
      pid = fork {
        if $$ == target_pid
          v2 = SecureRandom.random_bytes(8)
          if v2 == target
            exit(1)
          else
            exit(2)
          end
        end
        exit(3)
      }
      pid, status = Process.waitpid2(pid)
      case status.exitstatus
      when 1
        raise 'returned same sequence for same PID'
      when 2
        return true
      end
    end
    false # not recycled?
  end
end

  def test_s_random_bytes_without_openssl
    begin
      require 'openssl'
    rescue LoadError
      return
    end
    begin
      load_path = $LOAD_PATH.dup
      loaded_features = $LOADED_FEATURES.dup
      openssl = Object.instance_eval { remove_const(:OpenSSL) }

      remove_feature('securerandom.rb')
      remove_feature('openssl.rb')
      Dir.mktmpdir do |dir|
        open(File.join(dir, 'openssl.rb'), 'w') { |f|
          f << 'raise LoadError'
        }
        $LOAD_PATH.unshift(dir)
        require 'securerandom'
        test_s_random_bytes
      end
    ensure
      $LOADED_FEATURES.replace(loaded_features)
      $LOAD_PATH.replace(load_path)
      Object.const_set(:OpenSSL, openssl)
    end
  end

  def test_s_hex
    assert_equal(16 * 2, @it.hex.size)
    33.times do |idx|
      assert_equal(idx * 2, @it.hex(idx).size)
      assert_equal(idx, @it.hex(idx).gsub(/(..)/) { [$1].pack('H*') }.size)
    end
  end

  def test_s_base64
    assert_equal(16, @it.base64.unpack('m*')[0].size)
    17.times do |idx|
      assert_equal(idx, @it.base64(idx).unpack('m*')[0].size)
    end
  end

  def test_s_urlsafe_base64
    safe = /[\n+\/]/
    65.times do |idx|
      assert_not_match(safe, @it.urlsafe_base64(idx))
    end
    # base64 can include unsafe byte
    10001.times do |idx|
      return if safe =~ @it.base64(idx)
    end
    flunk
  end

  def test_s_random_number_float
    101.times do
      v = @it.random_number
      assert(0.0 <= v && v < 1.0)
    end
  end

  def test_s_random_number_float_by_zero
    101.times do
      v = @it.random_number(0)
      assert(0.0 <= v && v < 1.0)
    end
  end

  def test_s_random_number_int
    101.times do |idx|
      next if idx.zero?
      v = @it.random_number(idx)
      assert(0 <= v && v < idx)
    end
  end

  def test_uuid
    uuid = @it.uuid
    assert_equal(36, uuid.size)
    uuid.unpack('a8xa4xa4xa4xa12').each do |e|
      assert_match(/^[0-9a-f]+$/, e)
    end
  end

  def protect
    begin
      yield
    rescue NotImplementedError
      # ignore
    end
  end

  def remove_feature(basename)
    $LOADED_FEATURES.delete_if { |path|
      if File.basename(path) == basename
        $LOAD_PATH.any? { |dir|
          File.exists?(File.join(dir, basename))
        }
      end
    }
  end

end
