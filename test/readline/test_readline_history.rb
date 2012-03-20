begin
  require "readline"
=begin
  class << Readline::HISTORY
    def []=(index, str)
      raise NotImplementedError
    end

    def pop
      raise NotImplementedError
    end

    def shift
      raise NotImplementedError
    end

    def delete_at(index)
      raise NotImplementedError
    end
  end
=end

=begin
  class << Readline::HISTORY
    def clear
      raise NotImplementedError
    end
  end
=end
rescue LoadError
else
  require "test/unit"
end

class Readline::TestHistory < Test::Unit::TestCase
  include Readline

  def setup
    HISTORY.clear
  end

  def test_safe_level_4
    method_args =
      [
       ["[]", [0]],
       ["[]=", [0, "s"]],
       ["\<\<", ["s"]],
       ["push", ["s"]],
       ["pop", []],
       ["shift", []],
       ["length", []],
       ["delete_at", [0]],
       ["clear", []],
      ]
    method_args.each do |method_name, args|
      assert_raise(SecurityError, NotImplementedError,
                    "method=<#{method_name}>") do
        Thread.start {
          $SAFE = 4
          HISTORY.send(method_name.to_sym, *args)
          assert(true)
        }.join
      end
    end

    assert_raise(SecurityError, NotImplementedError,
                  "method=<each>") do
      Thread.start {
        $SAFE = 4
        HISTORY.each { |s|
          assert(true)
        }
      }.join
    end
  end

  def test_to_s
    expected = "HISTORY"
    assert_equal(expected, HISTORY.to_s)
  end

  def test_get
    lines = push_history(5)
    lines.each_with_index do |s, i|
      assert_external_string_equal(s, HISTORY[i])
    end
  end

  def test_get__negative
    lines = push_history(5)
    (1..5).each do |i|
      assert_equal(lines[-i], HISTORY[-i])
    end
  end

  def test_get__out_of_range
    lines = push_history(5)
    invalid_indexes = [5, 6, 100, -6, -7, -100]
    invalid_indexes.each do |i|
      assert_raise(IndexError, "i=<#{i}>") do
        HISTORY[i]
      end
    end

    invalid_indexes = [100_000_000_000_000_000_000,
                       -100_000_000_000_000_000_000]
    invalid_indexes.each do |i|
      assert_raise(RangeError, "i=<#{i}>") do
        HISTORY[i]
      end
    end
  end

  def test_set
    begin
      lines = push_history(5)
      5.times do |i|
        expected = "set: #{i}"
        HISTORY[i] = expected
        assert_external_string_equal(expected, HISTORY[i])
      end
    rescue NotImplementedError
    end
  end

  def test_set__out_of_range
    assert_raise(IndexError, NotImplementedError, "index=<0>") do
      HISTORY[0] = "set: 0"
    end

    lines = push_history(5)
    invalid_indexes = [5, 6, 100, -6, -7, -100]
    invalid_indexes.each do |i|
      assert_raise(IndexError, NotImplementedError, "index=<#{i}>") do
        HISTORY[i] = "set: #{i}"
      end
    end

    invalid_indexes = [100_000_000_000_000_000_000,
                       -100_000_000_000_000_000_000]
    invalid_indexes.each do |i|
      assert_raise(RangeError, NotImplementedError, "index=<#{i}>") do
        HISTORY[i] = "set: #{i}"
      end
    end
  end

  def test_push
    5.times do |i|
      s = i.to_s
      assert_equal(HISTORY, HISTORY.push(s))
      assert_external_string_equal(s, HISTORY[i])
    end
    assert_equal(5, HISTORY.length)
  end

  def test_push__operator
    5.times do |i|
      s = i.to_s
      assert_equal(HISTORY, HISTORY << s)
      assert_external_string_equal(s, HISTORY[i])
    end
    assert_equal(5, HISTORY.length)
  end

  def test_push__plural
    assert_equal(HISTORY, HISTORY.push("0", "1", "2", "3", "4"))
    (0..4).each do |i|
      assert_external_string_equal(i.to_s, HISTORY[i])
    end
    assert_equal(5, HISTORY.length)

    assert_equal(HISTORY, HISTORY.push("5", "6", "7", "8", "9"))
    (5..9).each do |i|
      assert_external_string_equal(i.to_s, HISTORY[i])
    end
    assert_equal(10, HISTORY.length)
  end

  def test_pop
    begin
      assert_equal(nil, HISTORY.pop)

      lines = push_history(5)
      (1..5).each do |i|
        assert_external_string_equal(lines[-i], HISTORY.pop)
        assert_equal(lines.length - i, HISTORY.length)
      end

      assert_equal(nil, HISTORY.pop)
    rescue NotImplementedError
    end
  end

  def test_shift
    begin
      assert_equal(nil, HISTORY.shift)

      lines = push_history(5)
      (0..4).each do |i|
        assert_external_string_equal(lines[i], HISTORY.shift)
        assert_equal(lines.length - (i + 1), HISTORY.length)
      end

      assert_equal(nil, HISTORY.shift)
    rescue NotImplementedError
    end
  end

  def test_each
    e = HISTORY.each do |s|
      assert(false) # not reachable
    end
    assert_equal(HISTORY, e)
    lines = push_history(5)
    i = 0
    e = HISTORY.each do |s|
      assert_external_string_equal(HISTORY[i], s)
      assert_external_string_equal(lines[i], s)
      i += 1
    end
    assert_equal(HISTORY, e)
  end

  def test_each__enumerator
    e = HISTORY.each
    assert_instance_of(Enumerator, e)
  end

  def test_length
    assert_equal(0, HISTORY.length)
    push_history(1)
    assert_equal(1, HISTORY.length)
    push_history(4)
    assert_equal(5, HISTORY.length)
    HISTORY.clear
    assert_equal(0, HISTORY.length)
  end

  def test_empty_p
    2.times do
      assert(HISTORY.empty?)
      HISTORY.push("s")
      assert_equal(false, HISTORY.empty?)
      HISTORY.clear
      assert(HISTORY.empty?)
    end
  end

  def test_delete_at
    begin
      lines = push_history(5)
      (0..4).each do |i|
        assert_external_string_equal(lines[i], HISTORY.delete_at(0))
      end
      assert(HISTORY.empty?)

      lines = push_history(5)
      (1..5).each do |i|
        assert_external_string_equal(lines[lines.length - i], HISTORY.delete_at(-1))
      end
      assert(HISTORY.empty?)

      lines = push_history(5)
      assert_external_string_equal(lines[0], HISTORY.delete_at(0))
      assert_external_string_equal(lines[4], HISTORY.delete_at(3))
      assert_external_string_equal(lines[1], HISTORY.delete_at(0))
      assert_external_string_equal(lines[3], HISTORY.delete_at(1))
      assert_external_string_equal(lines[2], HISTORY.delete_at(0))
      assert(HISTORY.empty?)
    rescue NotImplementedError
    end
  end

  def test_delete_at__out_of_range
    assert_raise(IndexError, NotImplementedError, "index=<0>") do
      HISTORY.delete_at(0)
    end

    lines = push_history(5)
    invalid_indexes = [5, 6, 100, -6, -7, -100]
    invalid_indexes.each do |i|
      assert_raise(IndexError, NotImplementedError, "index=<#{i}>") do
        HISTORY.delete_at(i)
      end
    end

    invalid_indexes = [100_000_000_000_000_000_000,
                       -100_000_000_000_000_000_000]
    invalid_indexes.each do |i|
      assert_raise(RangeError, NotImplementedError, "index=<#{i}>") do
        HISTORY.delete_at(i)
      end
    end
  end

  private

  def push_history(num)
    lines = []
    num.times do |i|
      s = "a"
      i.times do
        s = s.succ
      end
      lines.push("#{i + 1}:#{s}")
    end
    HISTORY.push(*lines)
    return lines
  end

  def assert_external_string_equal(expected, actual)
    assert_equal(expected, actual)
    assert_equal(get_default_internal_encoding, actual.encoding)
  end

  def get_default_internal_encoding
    return Encoding.default_internal || Encoding.find("locale")
  end
end if defined?(::Readline) && defined?(::Readline::HISTORY) &&
  (
   begin
     Readline::HISTORY.clear
   rescue NotImplementedError
     false
   end
   )
