require 'rubygems/test_case'
require 'rubygems/indexer'

unless defined?(Builder::XChar) then
  warn "Gem::Indexer tests are being skipped.  Install builder gem." if $VERBOSE
end

class TestGemIndexer < Gem::TestCase

  def setup
    super

    util_clear_gems
    util_make_gems

    @d2_0 = quick_spec 'd', '2.0' do |s|
      s.date = Gem::Specification::TODAY - 86400 * 3
    end
    util_build_gem @d2_0

    @d2_0_a = quick_spec 'd', '2.0.a'
    util_build_gem @d2_0_a

    @d2_0_b = quick_spec 'd', '2.0.b'
    util_build_gem @d2_0_b

    @tempdir = File.join(@tempdir, 'indexer')

    gems = File.join(@tempdir, 'gems')
    FileUtils.mkdir_p gems
    FileUtils.mv Dir[File.join(@gemhome, "cache", '*.gem')], gems

    @indexer = Gem::Indexer.new(@tempdir,
                                :rss_title     => 'ExampleForge gems',
                                :rss_host      => 'example.com',
                                :rss_gems_host => 'gems.example.com')
  end

  def test_initialize
    assert_equal @tempdir, @indexer.dest_directory
    assert_equal File.join(Dir.tmpdir, "gem_generate_index_#{$$}"),
                 @indexer.directory

    indexer = Gem::Indexer.new @tempdir
    assert indexer.build_legacy
    assert indexer.build_modern

    indexer = Gem::Indexer.new @tempdir, :build_legacy => false,
                               :build_modern => true
    refute indexer.build_legacy
    assert indexer.build_modern

    indexer = Gem::Indexer.new @tempdir, :build_legacy => true,
                               :build_modern => false
    assert indexer.build_legacy
    refute indexer.build_modern
  end

  def test_build_indicies
    @indexer.make_temp_directories

    use_ui @ui do
      @indexer.build_indicies
    end

    specs_path = File.join @indexer.directory, "specs.#{@marshal_version}"
    specs_dump = Gem.read_binary specs_path
    specs = Marshal.load specs_dump

    expected = [["a",      Gem::Version.new("1"),   "ruby"],
                ["a",      Gem::Version.new("2"),   "ruby"],
                ["a_evil", Gem::Version.new("9"),   "ruby"],
                ["b",      Gem::Version.new("2"),   "ruby"],
                ["c",      Gem::Version.new("1.2"), "ruby"],
                ["d",      Gem::Version.new("2.0"), "ruby"],
                ["pl",     Gem::Version.new("1"),   "i386-linux"]]

    assert_equal expected, specs

    latest_specs_path = File.join(@indexer.directory,
                                  "latest_specs.#{@marshal_version}")
    latest_specs_dump = Gem.read_binary latest_specs_path
    latest_specs = Marshal.load latest_specs_dump

    expected = [["a",      Gem::Version.new("2"),   "ruby"],
                ["a_evil", Gem::Version.new("9"),   "ruby"],
                ["b",      Gem::Version.new("2"),   "ruby"],
                ["c",      Gem::Version.new("1.2"), "ruby"],
                ["d",      Gem::Version.new("2.0"), "ruby"],
                ["pl",     Gem::Version.new("1"),   "i386-linux"]]

    assert_equal expected, latest_specs, 'latest_specs'
  end

  def test_generate_index
    use_ui @ui do
      @indexer.generate_index
    end

    assert_indexed @tempdir, "Marshal.#{@marshal_version}"
    assert_indexed @tempdir, "Marshal.#{@marshal_version}.Z"

    quickdir = File.join @tempdir, 'quick'
    marshal_quickdir = File.join quickdir, "Marshal.#{@marshal_version}"

    assert File.directory?(quickdir)
    assert File.directory?(marshal_quickdir)

    assert_indexed marshal_quickdir, "#{File.basename(@a1.spec_file)}.rz"
    assert_indexed marshal_quickdir, "#{File.basename(@a2.spec_file)}.rz"

    refute_indexed marshal_quickdir, File.basename(@c1_2.spec_file)

    assert_indexed @tempdir, "specs.#{@marshal_version}"
    assert_indexed @tempdir, "specs.#{@marshal_version}.gz"

    assert_indexed @tempdir, "latest_specs.#{@marshal_version}"
    assert_indexed @tempdir, "latest_specs.#{@marshal_version}.gz"

    expected = <<-EOF
<?xml version=\"1.0\"?>
<rss version=\"2.0\">
  <channel>
    <title>ExampleForge gems</title>
    <link>http://example.com</link>
    <description>Recently released gems from http://example.com</description>
    <generator>RubyGems v#{Gem::VERSION}</generator>
    <docs>http://cyber.law.harvard.edu/rss/rss.html</docs>
    <item>
      <title>a-2</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>a-2</guid>
      <enclosure url=\"http://gems.example.com/gems/a-2.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@a2.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>a-3.a</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>a-3.a</guid>
      <enclosure url=\"http://gems.example.com/gems/a-3.a.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@a3a.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>a_evil-9</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>a_evil-9</guid>
      <enclosure url=\"http://gems.example.com/gems/a_evil-9.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@a_evil9.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>b-2</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>b-2</guid>
      <enclosure url=\"http://gems.example.com/gems/b-2.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@b2.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>c-1.2</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>c-1.2</guid>
      <enclosure url=\"http://gems.example.com/gems/c-1.2.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@c1_2.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>d-2.0.a</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>d-2.0.a</guid>
      <enclosure url=\"http://gems.example.com/gems/d-2.0.a.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@d2_0_a.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>d-2.0.b</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>d-2.0.b</guid>
      <enclosure url=\"http://gems.example.com/gems/d-2.0.b.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@d2_0_b.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>pl-1-x86-linux</title>
      <description>
&lt;pre&gt;This is a test description&lt;/pre&gt;
      </description>
      <author>example@example.com (A User)</author>
      <guid>pl-1-x86-linux</guid>
      <enclosure url=\"http://gems.example.com/gems/pl-1-x86-linux.gem\"
                 length=\"3072\" type=\"application/octet-stream\" />
      <pubDate>#{@pl1.date.rfc2822}</pubDate>
      <link>http://example.com</link>
    </item>
    <item>
      <title>a-1</title>
      <description>
&lt;pre&gt;This line is really, really long.  So long, in fact, that it is more than
eighty characters long!  The purpose of this line is for testing wrapping
behavior because sometimes people don't wrap their text to eighty characters.
Without the wrapping, the text might not look good in the RSS feed.

Also, a list:
  * An entry that's actually kind of sort
  * an entry that's really long, which will probably get wrapped funny.
That's ok, somebody wasn't thinking straight when they made it more than
eighty characters.&lt;/pre&gt;
      </description>
      <author>example@example.com (Example), example2@example.com (Example2)</author>
      <guid>a-1</guid>
      <enclosure url=\"http://gems.example.com/gems/a-1.gem\"
                 length=\"3584\" type=\"application/octet-stream\" />
      <pubDate>#{@a1.date.rfc2822}</pubDate>
      <link>http://a.example.com</link>
    </item>
  </channel>
</rss>
    EOF

    gems_rss = File.read File.join(@tempdir, 'index.rss')

    assert_equal expected, gems_rss
  end

  def test_generate_index_legacy
    @indexer.build_modern = false
    @indexer.build_legacy = true

    use_ui @ui do
      @indexer.generate_index
    end

    assert_indexed @tempdir, "Marshal.#{@marshal_version}"
    assert_indexed @tempdir, "Marshal.#{@marshal_version}.Z"

    quickdir = File.join @tempdir, 'quick'
    marshal_quickdir = File.join quickdir, "Marshal.#{@marshal_version}"

    assert File.directory?(quickdir)
    assert File.directory?(marshal_quickdir)

    assert_indexed marshal_quickdir, "#{File.basename(@a1.spec_file)}.rz"
    assert_indexed marshal_quickdir, "#{File.basename(@a2.spec_file)}.rz"

    refute_indexed marshal_quickdir, "#{File.basename(@c1_2.spec_file)}"

    refute_indexed @tempdir, "specs.#{@marshal_version}"
    refute_indexed @tempdir, "specs.#{@marshal_version}.gz"

    refute_indexed @tempdir, "latest_specs.#{@marshal_version}"
    refute_indexed @tempdir, "latest_specs.#{@marshal_version}.gz"
  end

  def test_generate_index_legacy_back_to_back
    @indexer.build_modern = true
    @indexer.build_legacy = true

    use_ui @ui do
      @indexer.generate_index
    end

    @indexer = Gem::Indexer.new @tempdir
    @indexer.build_modern = false
    @indexer.build_legacy = true

    use_ui @ui do
      @indexer.generate_index
    end

    assert_indexed @tempdir, "Marshal.#{@marshal_version}"
    assert_indexed @tempdir, "Marshal.#{@marshal_version}.Z"

    quickdir = File.join @tempdir, 'quick'
    marshal_quickdir = File.join quickdir, "Marshal.#{@marshal_version}"

    assert File.directory?(marshal_quickdir)

    assert_indexed marshal_quickdir, "#{File.basename(@a1.spec_file)}.rz"
    assert_indexed marshal_quickdir, "#{File.basename(@a2.spec_file)}.rz"

    assert_indexed @tempdir, "specs.#{@marshal_version}"
    assert_indexed @tempdir, "specs.#{@marshal_version}.gz"

    assert_indexed @tempdir, "latest_specs.#{@marshal_version}"
    assert_indexed @tempdir, "latest_specs.#{@marshal_version}.gz"
  end

  def test_generate_index_modern
    @indexer.build_modern = true
    @indexer.build_legacy = false

    use_ui @ui do
      @indexer.generate_index
    end

    refute_indexed @tempdir, 'yaml'
    refute_indexed @tempdir, 'yaml.Z'
    refute_indexed @tempdir, "Marshal.#{@marshal_version}"
    refute_indexed @tempdir, "Marshal.#{@marshal_version}.Z"

    quickdir = File.join @tempdir, 'quick'
    marshal_quickdir = File.join quickdir, "Marshal.#{@marshal_version}"

    assert File.directory?(quickdir), 'quickdir should be directory'
    assert File.directory?(marshal_quickdir)

    refute_indexed quickdir, "index"
    refute_indexed quickdir, "index.rz"

    refute_indexed quickdir, "latest_index"
    refute_indexed quickdir, "latest_index.rz"

    refute_indexed quickdir, "#{File.basename(@a1.spec_file)}.rz"
    refute_indexed quickdir, "#{File.basename(@a2.spec_file)}.rz"
    refute_indexed quickdir, "#{File.basename(@b2.spec_file)}.rz"
    refute_indexed quickdir, "#{File.basename(@c1_2.spec_file)}.rz"

    refute_indexed quickdir, "#{@pl1.original_name}.gemspec.rz"
    refute_indexed quickdir, "#{File.basename(@pl1.spec_file)}.rz"

    assert_indexed marshal_quickdir, "#{File.basename(@a1.spec_file)}.rz"
    assert_indexed marshal_quickdir, "#{File.basename(@a2.spec_file)}.rz"

    refute_indexed quickdir, "#{File.basename(@c1_2.spec_file)}"
    refute_indexed marshal_quickdir, "#{File.basename(@c1_2.spec_file)}"

    assert_indexed @tempdir, "specs.#{@marshal_version}"
    assert_indexed @tempdir, "specs.#{@marshal_version}.gz"

    assert_indexed @tempdir, "latest_specs.#{@marshal_version}"
    assert_indexed @tempdir, "latest_specs.#{@marshal_version}.gz"
  end

  def test_generate_index_modern_back_to_back
    @indexer.build_modern = true
    @indexer.build_legacy = true

    use_ui @ui do
      @indexer.generate_index
    end

    @indexer = Gem::Indexer.new @tempdir
    @indexer.build_modern = true
    @indexer.build_legacy = false

    use_ui @ui do
      @indexer.generate_index
    end

    assert_indexed @tempdir, "Marshal.#{@marshal_version}"
    assert_indexed @tempdir, "Marshal.#{@marshal_version}.Z"

    quickdir = File.join @tempdir, 'quick'
    marshal_quickdir = File.join quickdir, "Marshal.#{@marshal_version}"

    assert File.directory?(quickdir)
    assert File.directory?(marshal_quickdir)

    assert_indexed marshal_quickdir, "#{File.basename(@a1.spec_file)}.rz"
    assert_indexed marshal_quickdir, "#{File.basename(@a2.spec_file)}.rz"

    assert_indexed @tempdir, "specs.#{@marshal_version}"
    assert_indexed @tempdir, "specs.#{@marshal_version}.gz"

    assert_indexed @tempdir, "latest_specs.#{@marshal_version}"
    assert_indexed @tempdir, "latest_specs.#{@marshal_version}.gz"
  end

  def test_generate_index_ui
    use_ui @ui do
      @indexer.generate_index
    end

    assert_match %r%^\.\.\.\.\.\.\.\.\.\.$%, @ui.output
    assert_match %r%^Generating Marshal quick index gemspecs for 10 gems$%,
                 @ui.output
    assert_match %r%^Complete$%, @ui.output
    assert_match %r%^Generating specs index$%, @ui.output
    assert_match %r%^Generating latest specs index$%, @ui.output
    assert_match %r%^Generating prerelease specs index$%, @ui.output
    assert_match %r%^Generating Marshal master index$%, @ui.output
    assert_match %r%^Complete$%, @ui.output
    assert_match %r%^Compressing indicies$%, @ui.output

    assert_equal '', @ui.error
  end

  def test_generate_index_specs
    use_ui @ui do
      @indexer.generate_index
    end

    specs_path = File.join @tempdir, "specs.#{@marshal_version}"

    specs_dump = Gem.read_binary specs_path
    specs = Marshal.load specs_dump

    expected = [
      ['a',      Gem::Version.new(1),     'ruby'],
      ['a',      Gem::Version.new(2),     'ruby'],
      ['a_evil', Gem::Version.new(9),     'ruby'],
      ['b',      Gem::Version.new(2),     'ruby'],
      ['c',      Gem::Version.new('1.2'), 'ruby'],
      ['d',      Gem::Version.new('2.0'), 'ruby'],
      ['pl',     Gem::Version.new(1),     'i386-linux'],
    ]

    assert_equal expected, specs

    assert_same specs[0].first, specs[1].first,
                'identical names not identical'

    assert_same specs[0][1],    specs[-1][1],
                'identical versions not identical'

    assert_same specs[0].last, specs[1].last,
                'identical platforms not identical'

    refute_same specs[1][1], specs[5][1],
                'different versions not different'
  end

  def test_generate_index_latest_specs
    use_ui @ui do
      @indexer.generate_index
    end

    latest_specs_path = File.join @tempdir, "latest_specs.#{@marshal_version}"

    latest_specs_dump = Gem.read_binary latest_specs_path
    latest_specs = Marshal.load latest_specs_dump

    expected = [
      ['a',      Gem::Version.new(2),     'ruby'],
      ['a_evil', Gem::Version.new(9),     'ruby'],
      ['b',      Gem::Version.new(2),     'ruby'],
      ['c',      Gem::Version.new('1.2'), 'ruby'],
      ['d',      Gem::Version.new('2.0'), 'ruby'],
      ['pl',     Gem::Version.new(1),     'i386-linux'],
    ]

    assert_equal expected, latest_specs

    assert_same latest_specs[0][1],   latest_specs[2][1],
                'identical versions not identical'

    assert_same latest_specs[0].last, latest_specs[1].last,
                'identical platforms not identical'
  end

  def test_generate_index_prerelease_specs
    use_ui @ui do
      @indexer.generate_index
    end

    prerelease_specs_path = File.join @tempdir, "prerelease_specs.#{@marshal_version}"

    prerelease_specs_dump = Gem.read_binary prerelease_specs_path
    prerelease_specs = Marshal.load prerelease_specs_dump

    assert_equal [['a', Gem::Version.new('3.a'),   'ruby'],
                  ['d', Gem::Version.new('2.0.a'), 'ruby'],
                  ['d', Gem::Version.new('2.0.b'), 'ruby']],
                 prerelease_specs
  end

  def test_update_index
    use_ui @ui do
      @indexer.generate_index
    end

    quickdir = File.join @tempdir, 'quick'
    marshal_quickdir = File.join quickdir, "Marshal.#{@marshal_version}"

    assert File.directory?(quickdir)
    assert File.directory?(marshal_quickdir)

    @d2_1 = quick_spec 'd', '2.1'
    util_build_gem @d2_1
    @d2_1_tuple = [@d2_1.name, @d2_1.version, @d2_1.original_platform]

    @d2_1_a = quick_spec 'd', '2.2.a'
    util_build_gem @d2_1_a
    @d2_1_a_tuple = [@d2_1_a.name, @d2_1_a.version, @d2_1_a.original_platform]

    gems = File.join @tempdir, 'gems'

    FileUtils.mv @d2_1.cache_file, gems
    FileUtils.mv @d2_1_a.cache_file, gems

    use_ui @ui do
      @indexer.update_index
    end

    assert_indexed marshal_quickdir, "#{File.basename(@d2_1.spec_file)}.rz"

    specs_index = Marshal.load Gem.read_binary(@indexer.dest_specs_index)

    assert_includes specs_index, @d2_1_tuple
    refute_includes specs_index, @d2_1_a_tuple

    latest_specs_index = Marshal.load \
      Gem.read_binary(@indexer.dest_latest_specs_index)

    assert_includes latest_specs_index, @d2_1_tuple
    assert_includes latest_specs_index,
                    [@d2_0.name, @d2_0.version, @d2_0.original_platform]
    refute_includes latest_specs_index, @d2_1_a_tuple

    pre_specs_index = Marshal.load \
      Gem.read_binary(@indexer.dest_prerelease_specs_index)

    assert_includes pre_specs_index, @d2_1_a_tuple
    refute_includes pre_specs_index, @d2_1_tuple
  end

  def assert_indexed(dir, name)
    file = File.join dir, name
    assert File.exist?(file), "#{file} does not exist"
  end

  def refute_indexed(dir, name)
    file = File.join dir, name
    refute File.exist?(file), "#{file} exists"
  end

end if defined?(Builder::XChar)

