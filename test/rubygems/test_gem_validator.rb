require 'rubygems/test_case'
require "rubygems/simple_gem"
require 'rubygems/validator'

class TestGemValidator < Gem::TestCase

  def setup
    super

    @simple_gem = SIMPLE_GEM
    @validator = Gem::Validator.new
  end

  def test_verify_gem_file
    gem_file = File.join @tempdir, 'simple_gem.gem'
    File.open gem_file, 'wb' do |fp| fp.write @simple_gem end

    assert_equal nil, @validator.verify_gem_file(gem_file)
  end

  def test_verify_gem_file_empty
    e = assert_raises Gem::VerificationError do
      @validator.verify_gem_file ''
    end

    assert_equal 'missing gem file ', e.message
  end

  def test_verify_gem_file_nonexistent
    file = '/nonexistent/nonexistent.gem'
    e = assert_raises Gem::VerificationError do
      @validator.verify_gem_file file
    end

    assert_equal "missing gem file #{file}", e.message
  end

  def test_verify_gem
    assert_equal nil, @validator.verify_gem(@simple_gem)
  end

  def test_verify_gem_empty
    e = assert_raises Gem::VerificationError do
      @validator.verify_gem ''
    end

    assert_equal 'empty gem file', e.message
  end

  def test_verify_gem_invalid_checksum
    e = assert_raises Gem::VerificationError do
      @validator.verify_gem @simple_gem.upcase
    end

    assert_equal 'invalid checksum for gem file', e.message
  end

  def test_verify_gem_no_sum
    assert_equal nil, @validator.verify_gem('words')
  end

end

