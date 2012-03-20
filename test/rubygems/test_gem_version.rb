require 'rubygems/test_case'
require "rubygems/version"

class TestGemVersion < Gem::TestCase

  def test_bump
    assert_bumped_version_equal "5.3", "5.2.4"
  end

  def test_bump_alpha
    assert_bumped_version_equal "5.3", "5.2.4.a"
  end

  def test_bump_alphanumeric
    assert_bumped_version_equal "5.3", "5.2.4.a10"
  end

  def test_bump_trailing_zeros
    assert_bumped_version_equal "5.1", "5.0.0"
  end

  def test_bump_one_level
    assert_bumped_version_equal "6", "5"
  end

  # FIX: For "legacy reasons," any object that responds to +version+
  # is returned unchanged. I'm not certain why.

  def test_class_create
    fake = Object.new
    def fake.version; "1.0" end

    assert_same  fake, Gem::Version.create(fake)
    assert_nil   Gem::Version.create(nil)
    assert_equal v("5.1"), Gem::Version.create("5.1")
  end

  def test_eql_eh
    assert_version_eql "1.2",    "1.2"
    refute_version_eql "1.2",    "1.2.0"
    refute_version_eql "1.2",    "1.3"
    refute_version_eql "1.2.b1", "1.2.b.1"
  end

  def test_equals2
    assert_version_equal "1.2",    "1.2"
    refute_version_equal "1.2",    "1.3"
    assert_version_equal "1.2.b1", "1.2.b.1"
  end

  # REVISIT: consider removing as too impl-bound
  def test_hash
    assert_equal v("1.2").hash, v("1.2").hash
    refute_equal v("1.2").hash, v("1.3").hash
    refute_equal v("1.2").hash, v("1.2.0").hash
  end

  def test_initialize
    ["1.0", "1.0 ", " 1.0 ", "1.0\n", "\n1.0\n"].each do |good|
      assert_version_equal "1.0", good
    end

    assert_version_equal "1", 1
  end

  def test_initialize_bad
    ["junk", "1.0\n2.0"].each do |bad|
      e = assert_raises ArgumentError do
        Gem::Version.new bad
      end

      assert_equal "Malformed version number string #{bad}", e.message
    end
  end

  def test_prerelease
    assert_prerelease "1.2.0.a"
    assert_prerelease "2.9.b"
    assert_prerelease "22.1.50.0.d"
    assert_prerelease "1.2.d.42"

    assert_prerelease '1.A'

    refute_prerelease "1.2.0"
    refute_prerelease "2.9"
    refute_prerelease "22.1.50.0"
  end

  def test_release
    assert_release_equal "1.2.0", "1.2.0.a"
    assert_release_equal "1.1",   "1.1.rc10"
    assert_release_equal "1.9.3", "1.9.3.alpha.5"
    assert_release_equal "1.9.3", "1.9.3"
  end

  def test_spaceship
    assert_equal( 0, v("1.0")       <=> v("1.0.0"))
    assert_equal( 1, v("1.0")       <=> v("1.0.a"))
    assert_equal( 1, v("1.8.2")     <=> v("0.0.0"))
    assert_equal( 1, v("1.8.2")     <=> v("1.8.2.a"))
    assert_equal( 1, v("1.8.2.b")   <=> v("1.8.2.a"))
    assert_equal(-1, v("1.8.2.a")   <=> v("1.8.2"))
    assert_equal( 1, v("1.8.2.a10") <=> v("1.8.2.a9"))
    assert_equal( 0, v("")          <=> v("0"))

    assert_nil v("1.0") <=> "whatever"
  end

  def test_spermy_recommendation
    assert_spermy_equal "~> 1.0", "1"
    assert_spermy_equal "~> 1.0", "1.0"
    assert_spermy_equal "~> 1.2", "1.2"
    assert_spermy_equal "~> 1.2", "1.2.0"
    assert_spermy_equal "~> 1.2", "1.2.3"
    assert_spermy_equal "~> 1.2", "1.2.3.a.4"
  end

  def test_to_s
    assert_equal "5.2.4", v("5.2.4").to_s
  end

  # Asserts that +version+ is a prerelease.

  def assert_prerelease version
    assert v(version).prerelease?, "#{version} is a prerelease"
  end

  # Assert that +expected+ is the "spermy" recommendation for +version".

  def assert_spermy_equal expected, version
    assert_equal expected, v(version).spermy_recommendation
  end

  # Assert that bumping the +unbumped+ version yields the +expected+.

  def assert_bumped_version_equal expected, unbumped
    assert_version_equal expected, v(unbumped).bump
  end

  # Assert that +release+ is the correct non-prerelease +version+.

  def assert_release_equal release, version
    assert_version_equal release, v(version).release
  end

  # Assert that two versions are equal. Handles strings or
  # Gem::Version instances.

  def assert_version_equal expected, actual
    assert_equal v(expected), v(actual)
  end

  # Assert that two versions are eql?. Checks both directions.

  def assert_version_eql first, second
    first, second = v(first), v(second)
    assert first.eql?(second), "#{first} is eql? #{second}"
    assert second.eql?(first), "#{second} is eql? #{first}"
  end

  # Refute the assumption that +version+ is a prerelease.

  def refute_prerelease version
    refute v(version).prerelease?, "#{version} is NOT a prerelease"
  end

  # Refute the assumption that two versions are eql?. Checks both
  # directions.

  def refute_version_eql first, second
    first, second = v(first), v(second)
    refute first.eql?(second), "#{first} is NOT eql? #{second}"
    refute second.eql?(first), "#{second} is NOT eql? #{first}"
  end

  # Refute the assumption that the two versions are equal?.

  def refute_version_equal unexpected, actual
    refute_equal v(unexpected), v(actual)
  end
end
