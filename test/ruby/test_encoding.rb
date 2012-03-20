require 'test/unit'
require_relative 'envutil'

class TestEncoding < Test::Unit::TestCase

  # Test basic encoding methods: list, find, name
  def test_encoding
    encodings = Encoding.list
    assert_equal(encodings.empty?, false)

    encodings.each do |e|
      assert_equal(e, Encoding.find(e.name))
      assert_equal(e, Encoding.find(e.name.upcase))
      assert_equal(e, Encoding.find(e.name.capitalize))
      assert_equal(e, Encoding.find(e.name.downcase))
      assert_equal(e, Encoding.find(e))
    end
  end

  def test_enc_names
    aliases = Encoding.aliases
    aliases.each do |a, en|
      e = Encoding.find(a)
      assert_equal(e.name, en)
      assert(e.names.include?(a))
    end
  end

  # Test that Encoding objects can't be copied
  # And that they can be compared by object_id
  def test_singleton
    encodings = Encoding.list
    encodings.each do |e|
      assert_raise(TypeError) { e.dup }
      assert_raise(TypeError) { e.clone }
      assert_equal(e.object_id, Marshal.load(Marshal.dump(e)).object_id)
    end
  end

  def test_find
    assert_raise(ArgumentError) { Encoding.find("foobarbazqux") }
    assert_nothing_raised{Encoding.find("locale")}
    assert_nothing_raised{Encoding.find("filesystem")}

    if /(?:ms|dar)win|mingw/ !~ RUBY_PLATFORM
      # Unix's filesystem encoding is default_external
      assert_ruby_status(%w[-EUTF-8:EUC-JP], <<-'EOS')
        exit Encoding.find("filesystem") == Encoding::UTF_8
        Encoding.default_external = Encoding::EUC_JP
        exit Encoding.find("filesystem") == Encoding::EUC_JP
      EOS
    end

    bug5150 = '[ruby-dev:44327]'
    assert_raise(TypeError, bug5150) {Encoding.find(1)}
  end

  def test_replicate
    assert_instance_of(Encoding, Encoding::UTF_8.replicate('UTF-8-ANOTHER'))
    assert_instance_of(Encoding, Encoding::ISO_2022_JP.replicate('ISO-2022-JP-ANOTHER'))
    bug3127 = '[ruby-dev:40954]'
    assert_raise(TypeError, bug3127) {Encoding::UTF_8.replicate(0)}
    assert_raise(ArgumentError, bug3127) {Encoding::UTF_8.replicate("\0")}
  end

  def test_dummy_p
    assert_equal(true, Encoding::ISO_2022_JP.dummy?)
    assert_equal(false, Encoding::UTF_8.dummy?)
  end

  def test_ascii_compatible_p
    assert_equal(true, Encoding::ASCII_8BIT.ascii_compatible?)
    assert_equal(true, Encoding::UTF_8.ascii_compatible?)
    assert_equal(false, Encoding::UTF_16BE.ascii_compatible?)
    assert_equal(false, Encoding::ISO_2022_JP.ascii_compatible?)
  end

  def test_name_list
    assert_instance_of(Array, Encoding.name_list)
    Encoding.name_list.each do |x|
      assert_instance_of(String, x)
    end
  end

  def test_aliases
    assert_instance_of(Hash, Encoding.aliases)
    Encoding.aliases.each do |k, v|
      assert(Encoding.name_list.include?(k))
      assert(Encoding.name_list.include?(v))
      assert_instance_of(String, k)
      assert_instance_of(String, v)
    end
  end

  def test_marshal
    str = "".force_encoding("EUC-JP")
    str2 = Marshal.load(Marshal.dump(str))
    assert_equal(str, str2)
    str2 = Marshal.load(Marshal.dump(str2))
    assert_equal(str, str2, '[ruby-dev:38596]')
  end

  def test_unsafe
    bug5279 = '[ruby-dev:44469]'
    assert_ruby_status([], '$SAFE=4; "a".encode("utf-16be")', bug5279)
  end

  def test_compatible_p
    ua = "abc".force_encoding(Encoding::UTF_8)
    assert_equal(Encoding::UTF_8, Encoding.compatible?(ua, :abc))
    assert_equal(nil, Encoding.compatible?(ua, 1))
    bin = "a".force_encoding(Encoding::ASCII_8BIT)
    asc = "b".force_encoding(Encoding::US_ASCII)
    assert_equal(Encoding::ASCII_8BIT, Encoding.compatible?(bin, asc))
  end
end
