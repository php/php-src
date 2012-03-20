require 'rubygems'
require 'minitest/autorun'
require 'rdoc/options'

require 'fileutils'
require 'tmpdir'

class TestRDocOptions < MiniTest::Unit::TestCase

  def setup
    @options = RDoc::Options.new
    @generators = RDoc::RDoc::GENERATORS.dup
  end

  def teardown
    RDoc::RDoc::GENERATORS.replace @generators
  end

  def test_check_files
    skip "assumes UNIX permission model" if /mswin|mingw/ =~ RUBY_PLATFORM
    out, err = capture_io do
      Dir.mktmpdir do |dir|
        Dir.chdir dir do
          FileUtils.touch 'unreadable'
          FileUtils.chmod 0, 'unreadable'

          @options.files = %w[nonexistent unreadable]

          @options.check_files
        end
      end
    end

    assert_empty @options.files

    assert_equal '', out

    expected = <<-EXPECTED
file 'nonexistent' not found
file 'unreadable' not readable
    EXPECTED

    assert_equal expected, err
  end

  def test_dry_run_default
    refute @options.dry_run
  end

  def test_encoding_default
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    assert_equal Encoding.default_external, @options.encoding
  end

  def test_generator_descriptions
    # HACK autotest/isolate should take care of this
    RDoc::RDoc::GENERATORS.clear
    RDoc::RDoc::GENERATORS['darkfish'] = RDoc::Generator::Darkfish
    RDoc::RDoc::GENERATORS['ri']       = RDoc::Generator::RI

    expected = <<-EXPECTED.chomp
  darkfish - HTML generator, written by Michael Granger
  ri       - creates ri data files
    EXPECTED

    assert_equal expected, @options.generator_descriptions
  end

  def test_parse_coverage
    @options.parse %w[--dcov]

    assert @options.coverage_report
    assert @options.force_update
  end

  def test_parse_coverage_no
    @options.parse %w[--no-dcov]

    refute @options.coverage_report
  end

  def test_parse_coverage_level_1
    @options.parse %w[--dcov=1]

    assert_equal 1, @options.coverage_report
  end

  def test_parse_dash_p
    out, err = capture_io do
      @options.parse %w[-p]
    end

    assert @options.pipe
    refute_match %r%^Usage: %, err
    refute_match %r%^invalid options%, err

    assert_empty out
  end

  def test_parse_dash_p_files
    out, err = capture_io do
      @options.parse ['-p', File.expand_path(__FILE__)]
    end

    refute @options.pipe
    refute_match %r%^Usage: %, err
    assert_match %r%^invalid options: -p .with files.%, err

    assert_empty out
  end

  def test_parse_default
    @options.parse []

    assert_equal RDoc::Generator::Darkfish,             @options.generator
    assert_equal 'darkfish',                            @options.template
    assert_match %r%rdoc/generator/template/darkfish$%, @options.template_dir
  end

  def test_parse_deprecated
    dep_hash = RDoc::Options::DEPRECATED
    options = dep_hash.keys.sort

    out, err = capture_io do
      @options.parse options
    end

    dep_hash.each_pair do |opt, message|
      assert_match %r%.*#{opt}.+#{message}%, err
    end

    assert_empty out
  end

  def test_parse_dry_run
    @options.parse %w[--dry-run]

    assert @options.dry_run
  end

  def test_parse_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    @options.parse %w[--encoding Big5]

    assert_equal Encoding::Big5, @options.encoding
    assert_equal 'Big5',         @options.charset
  end

  def test_parse_encoding_invalid
    skip "Encoding not implemented" unless Object.const_defined? :Encoding

    out, err = capture_io do
      @options.parse %w[--encoding invalid]
    end

    assert_match %r%^invalid options: --encoding invalid%, err

    assert_empty out
  end

  def test_parse_formatter
    e = assert_raises OptionParser::InvalidOption do
      @options.parse %w[--format darkfish --format ri]
    end

    assert_equal 'invalid option: --format generator already set to darkfish',
                 e.message
  end

  def test_parse_formatter_ri
    e = assert_raises OptionParser::InvalidOption do
      @options.parse %w[--format darkfish --ri]
    end

    assert_equal 'invalid option: --ri generator already set to darkfish',
                 e.message

    @options = RDoc::Options.new

    e = assert_raises OptionParser::InvalidOption do
      @options.parse %w[--format darkfish -r]
    end

    assert_equal 'invalid option: -r generator already set to darkfish',
                 e.message
  end

  def test_parse_formatter_ri_site
    e = assert_raises OptionParser::InvalidOption do
      @options.parse %w[--format darkfish --ri-site]
    end

    assert_equal 'invalid option: --ri-site generator already set to darkfish',
                 e.message

    @options = RDoc::Options.new

    e = assert_raises OptionParser::InvalidOption do
      @options.parse %w[--format darkfish -R]
    end

    assert_equal 'invalid option: -R generator already set to darkfish',
                 e.message
  end

  def test_parse_help
    out, = capture_io do
      begin
        @options.parse %w[--help]
      rescue SystemExit
      end
    end

    assert_equal 1, out.scan(/HTML generator options:/).length
    assert_equal 1, out.scan(/ri generator options:/).  length
  end

  def test_parse_help_extra_generator
    RDoc::RDoc::GENERATORS['test'] = Class.new do
      def self.setup_options options
        op = options.option_parser

        op.separator 'test generator options:'
      end
    end

    out, = capture_io do
      begin
        @options.parse %w[--help]
      rescue SystemExit
      end
    end

    assert_equal 1, out.scan(/HTML generator options:/).length
    assert_equal 1, out.scan(/ri generator options:/).  length
    assert_equal 1, out.scan(/test generator options:/).length
  end

  def test_parse_ignore_invalid
    out, err = capture_io do
      @options.parse %w[--ignore-invalid --bogus]
    end

    refute_match %r%^Usage: %, err
    assert_match %r%^invalid options: --bogus%, err

    assert_empty out
  end

  def test_parse_ignore_invalid_default
    out, err = capture_io do
      @options.parse %w[--bogus --main BLAH]
    end

    refute_match %r%^Usage: %, err
    assert_match %r%^invalid options: --bogus%, err

    assert_equal 'BLAH', @options.main_page

    assert_empty out
  end

  def test_parse_ignore_invalid_no
    out, err = capture_io do
      assert_raises SystemExit do
        @options.parse %w[--no-ignore-invalid --bogus=arg --bobogus --visibility=extended]
      end
    end

    assert_match %r%^Usage: %, err
    assert_match %r%^invalid options: --bogus=arg, --bobogus, --visibility=extended%, err

    assert_empty out
  end

  def test_parse_main
    out, err = capture_io do
      @options.parse %w[--main MAIN]
    end

    assert_empty out
    assert_empty err

    assert_equal 'MAIN', @options.main_page
  end

  def test_parse_template
    out, err = capture_io do
      @options.parse %w[--template darkfish]
    end

    assert_empty out
    assert_empty err

    assert_equal 'darkfish', @options.template

    assert_match %r%rdoc/generator/template/darkfish$%, @options.template_dir
  end

  def test_parse_template_nonexistent
    out, err = capture_io do
      @options.parse %w[--template NONEXISTENT]
    end

    assert_empty out
    assert_equal "could not find template NONEXISTENT\n", err

    assert_equal 'darkfish', @options.template
    assert_match %r%rdoc/generator/template/darkfish$%, @options.template_dir
  end

  def test_parse_template_load_path
    orig_LOAD_PATH = $LOAD_PATH.dup

    template_dir = nil

    Dir.mktmpdir do |dir|
      $LOAD_PATH << dir

      template_dir = File.join dir, 'rdoc', 'generator', 'template', 'load_path'

      FileUtils.mkdir_p template_dir

      out, err = capture_io do
        @options.parse %w[--template load_path]
      end

      assert_empty out
      assert_empty err
    end

    assert_equal 'load_path',  @options.template
    assert_equal template_dir, @options.template_dir
  ensure
    $LOAD_PATH.replace orig_LOAD_PATH
  end

  def test_setup_generator
    test_generator = Class.new do
      def self.setup_options op
        @op = op
      end

      def self.op() @op end
    end

    RDoc::RDoc::GENERATORS['test'] = test_generator

    @options.setup_generator 'test'

    assert_equal test_generator, @options.generator
    assert_equal [test_generator], @options.generator_options

    assert_equal @options, test_generator.op
  ensure
    RDoc::RDoc::GENERATORS.delete 'test'
  end

  def test_setup_generator_no_option_parser
    test_generator = Class.new do
      def self.setup_options op
        op.option_parser.separator nil
        @op = op
      end

      def self.op() @op end
    end

    RDoc::RDoc::GENERATORS['test'] = test_generator

    @options.setup_generator 'test'

    assert_equal test_generator, @options.generator
    assert_equal [test_generator], @options.generator_options

    assert_equal @options, test_generator.op
  ensure
    RDoc::RDoc::GENERATORS.delete 'test'
  end

  def test_update_output_dir
    assert @options.update_output_dir

    @options.update_output_dir = false

    refute @options.update_output_dir
  end

end

