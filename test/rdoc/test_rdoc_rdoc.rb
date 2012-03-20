require 'rubygems'
require 'minitest/autorun'
require 'rdoc/rdoc'

require 'fileutils'
require 'tempfile'
require 'tmpdir'

class TestRDocRDoc < MiniTest::Unit::TestCase

  def setup
    RDoc::TopLevel.reset

    @rdoc = RDoc::RDoc.new
    @rdoc.options = RDoc::Options.new

    @stats = RDoc::Stats.new 0, 0
    @rdoc.instance_variable_set :@stats, @stats
  end

  def test_class_reset
    tl = RDoc::TopLevel.new 'file.rb'
    tl.add_class RDoc::NormalClass, 'C'
    tl.add_class RDoc::NormalModule, 'M'

    c = RDoc::Parser::C
    enclosure_classes = c.send :class_variable_get, :@@enclosure_classes
    enclosure_classes['A'] = 'B'
    known_bodies = c.send :class_variable_get, :@@known_bodies
    known_bodies['A'] = 'B'

    RDoc::RDoc.reset

    assert_empty RDoc::TopLevel.all_classes_hash
    assert_empty RDoc::TopLevel.all_files_hash
    assert_empty RDoc::TopLevel.all_modules_hash

    assert_empty c.send :class_variable_get, :@@enclosure_classes
    assert_empty c.send :class_variable_get, :@@known_bodies
  end

  def test_gather_files
    file = File.expand_path __FILE__
    assert_equal [file], @rdoc.gather_files([file, file])
  end

  def test_normalized_file_list
    files = @rdoc.normalized_file_list [__FILE__]

    files = files.map { |file| File.expand_path file }

    assert_equal [File.expand_path(__FILE__)], files
  end

  def test_normalized_file_list_not_modified
    files = [__FILE__]

    @rdoc.last_modified[__FILE__] = File.stat(__FILE__).mtime

    files = @rdoc.normalized_file_list [__FILE__]

    assert_empty files
  end

  def test_parse_file_encoding
    skip "Encoding not implemented" unless Object.const_defined? :Encoding
    @rdoc.options.encoding = Encoding::ISO_8859_1

    Tempfile.open 'test.txt' do |io|
      io.write 'hi'
      io.rewind

      top_level = @rdoc.parse_file io.path

      assert_equal Encoding::ISO_8859_1, top_level.absolute_name.encoding
    end
  end

  def test_remove_unparseable
    file_list = %w[
      blah.class
      blah.eps
      blah.erb
      blah.scpt.txt
      blah.ttf
      blah.yml
    ]

    assert_empty @rdoc.remove_unparseable file_list
  end

  def test_setup_output_dir
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir {|d|
      path = File.join d, 'testdir'

      last = @rdoc.setup_output_dir path, false

      assert_empty last

      assert File.directory? path
      assert File.exist? @rdoc.output_flag_file path
    }
  end

  def test_setup_output_dir_dry_run
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    @rdoc.options.dry_run = true

    Dir.mktmpdir do |d|
      path = File.join d, 'testdir'

      @rdoc.setup_output_dir path, false

      refute File.exist? path
    end
  end

  def test_setup_output_dir_exists
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir {|path|
      open @rdoc.output_flag_file(path), 'w' do |io|
        io.puts Time.at 0
        io.puts "./lib/rdoc.rb\t#{Time.at 86400}"
      end

      last = @rdoc.setup_output_dir path, false

      assert_equal 1, last.size
      assert_equal Time.at(86400), last['./lib/rdoc.rb']
    }
  end

  def test_setup_output_dir_exists_empty_created_rid
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir {|path|
      open @rdoc.output_flag_file(path), 'w' do end

      e = assert_raises RDoc::Error do
        @rdoc.setup_output_dir path, false
      end

      assert_match %r%Directory #{Regexp.escape path} already exists%, e.message
    }
  end

  def test_setup_output_dir_exists_file
    Tempfile.open 'test_rdoc_rdoc' do |tempfile|
      path = tempfile.path

      e = assert_raises RDoc::Error do
        @rdoc.setup_output_dir path, false
      end

      assert_match(%r%#{Regexp.escape path} exists and is not a directory%,
                   e.message)
    end
  end

  def test_setup_output_dir_exists_not_rdoc
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir do |dir|
      e = assert_raises RDoc::Error do
        @rdoc.setup_output_dir dir, false
      end

      assert_match %r%Directory #{Regexp.escape dir} already exists%, e.message
    end
  end

  def test_update_output_dir
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir do |d|
      @rdoc.update_output_dir d, Time.now, {}

      assert File.exist? "#{d}/created.rid"
    end
  end

  def test_update_output_dir_dont
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir do |d|
      @rdoc.options.update_output_dir = false
      @rdoc.update_output_dir d, Time.now, {}

      refute File.exist? "#{d}/created.rid"
    end
  end

  def test_update_output_dir_dry_run
    skip "No Dir::mktmpdir, upgrade your ruby" unless Dir.respond_to? :mktmpdir

    Dir.mktmpdir do |d|
      @rdoc.options.dry_run = true
      @rdoc.update_output_dir d, Time.now, {}

      refute File.exist? "#{d}/created.rid"
    end
  end

end

