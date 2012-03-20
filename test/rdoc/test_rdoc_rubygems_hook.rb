require 'rubygems/test_case'
require 'rubygems'
require 'rdoc/rubygems_hook'

class TestRDocRubygemsHook < Gem::TestCase

  def setup
    super

    @a = quick_spec 'a'

    @rdoc = RDoc::RubygemsHook.new @a

    begin
      RDoc::RubygemsHook.load_rdoc
    rescue Gem::DocumentError => e
      skip e.message
    end

    Gem.configuration[:rdoc] = nil
  end

  def test_initialize
    assert @rdoc.generate_rdoc
    assert @rdoc.generate_ri

    rdoc = RDoc::RubygemsHook.new @a, false, false

    refute rdoc.generate_rdoc
    refute rdoc.generate_ri
  end

  def test_delete_legacy_args
    args = %w[
      --inline-source
      --one-file
      --promiscuous
      -p
    ]

    @rdoc.delete_legacy_args args

    assert_empty args
  end

  def test_document
    options = RDoc::Options.new
    options.files = []

    @rdoc.instance_variable_set :@rdoc, @rdoc.new_rdoc
    @rdoc.instance_variable_set :@file_info, []

    @rdoc.document 'darkfish', options, @a.doc_dir('rdoc')

    assert @rdoc.rdoc_installed?
  end

  def test_generate
    FileUtils.mkdir_p @a.doc_dir
    FileUtils.mkdir_p File.join(@a.gem_dir, 'lib')

    @rdoc.generate

    assert @rdoc.rdoc_installed?
    assert @rdoc.ri_installed?

    rdoc = @rdoc.instance_variable_get :@rdoc

    refute rdoc.options.hyperlink_all
  end

  def test_generate_configuration_rdoc_array
    Gem.configuration[:rdoc] = %w[-A]

    FileUtils.mkdir_p @a.doc_dir
    FileUtils.mkdir_p File.join(@a.gem_dir, 'lib')

    @rdoc.generate

    rdoc = @rdoc.instance_variable_get :@rdoc

    assert rdoc.options.hyperlink_all
  end

  def test_generate_configuration_rdoc_string
    Gem.configuration[:rdoc] = '-A'

    FileUtils.mkdir_p @a.doc_dir
    FileUtils.mkdir_p File.join(@a.gem_dir, 'lib')

    @rdoc.generate

    rdoc = @rdoc.instance_variable_get :@rdoc

    assert rdoc.options.hyperlink_all
  end

  def test_generate_disabled
    @rdoc.generate_rdoc = false
    @rdoc.generate_ri   = false

    @rdoc.generate

    refute @rdoc.rdoc_installed?
    refute @rdoc.ri_installed?
  end

  def test_generate_force
    FileUtils.mkdir_p @a.doc_dir 'ri'
    FileUtils.mkdir_p @a.doc_dir 'rdoc'
    FileUtils.mkdir_p File.join(@a.gem_dir, 'lib')

    @rdoc.force = true

    @rdoc.generate

    assert_path_exists File.join(@a.doc_dir('rdoc'), 'index.html')
    assert_path_exists File.join(@a.doc_dir('ri'),   'cache.ri')
  end

  def test_generate_no_overwrite
    FileUtils.mkdir_p @a.doc_dir 'ri'
    FileUtils.mkdir_p @a.doc_dir 'rdoc'
    FileUtils.mkdir_p File.join(@a.gem_dir, 'lib')

    @rdoc.generate

    refute_path_exists File.join(@a.doc_dir('rdoc'), 'index.html')
    refute_path_exists File.join(@a.doc_dir('ri'),   'cache.ri')
  end

  def test_new_rdoc
    assert_kind_of RDoc::RDoc, @rdoc.new_rdoc
  end

  def test_rdoc_installed?
    refute @rdoc.rdoc_installed?

    FileUtils.mkdir_p @a.doc_dir 'rdoc'

    assert @rdoc.rdoc_installed?
  end

  def test_remove
    FileUtils.mkdir_p @a.doc_dir 'rdoc'
    FileUtils.mkdir_p @a.doc_dir 'ri'

    @rdoc.remove

    refute @rdoc.rdoc_installed?
    refute @rdoc.ri_installed?

    assert_path_exists @a.doc_dir
  end

  def test_remove_unwritable
    skip 'chmod not supported' if Gem.win_platform?
    FileUtils.mkdir_p @a.base_dir
    FileUtils.chmod 0, @a.base_dir

    e = assert_raises Gem::FilePermissionError do
      @rdoc.remove
    end

    assert_equal @a.base_dir, e.directory
  ensure
    FileUtils.chmod 0755, @a.base_dir
  end

  def test_ri_installed?
    refute @rdoc.ri_installed?

    FileUtils.mkdir_p @a.doc_dir 'ri'

    assert @rdoc.ri_installed?
  end

  def test_setup
    @rdoc.setup

    assert_path_exists @a.doc_dir
  end

  def test_setup_unwritable
    skip 'chmod not supported' if Gem.win_platform?
    FileUtils.mkdir_p @a.doc_dir
    FileUtils.chmod 0, @a.doc_dir

    e = assert_raises Gem::FilePermissionError do
      @rdoc.setup
    end

    assert_equal @a.doc_dir, e.directory
  ensure
    FileUtils.chmod 0755, @a.doc_dir
  end

end if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.9')

