require 'rubygems/test_case'
require 'rubygems/builder'
require 'rubygems/package'

class TestGemBuilder < Gem::TestCase

  def test_build
    builder = Gem::Builder.new quick_spec('a')

    use_ui @ui do
      Dir.chdir @tempdir do
        builder.build
      end
    end

    assert_match %r|Successfully built RubyGem\n  Name: a|, @ui.output
  end

  def test_build_validates
    builder = Gem::Builder.new Gem::Specification.new

    assert_raises Gem::InvalidSpecificationException do
      builder.build
    end
  end

  def test_build_specification_result
    util_make_gems

    spec = build_gem_and_yield_spec @a1

    assert_operator @a1, :eql?, spec
  end

  def build_gem_and_yield_spec(spec)
    builder = Gem::Builder.new spec

    spec = Dir.chdir @tempdir do
      FileUtils.mkdir 'lib'
      File.open('lib/code.rb', 'w') { |f| f << "something" }
      Gem::Package.open(File.open(builder.build, 'rb')) { |x| x.metadata }
    end
  end
end
