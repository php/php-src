require 'rubygems/test_case'
require 'rubygems/platform'
require 'rbconfig'

class TestGemPlatform < Gem::TestCase

  def test_self_local
    util_set_arch 'i686-darwin8.10.1'

    assert_equal Gem::Platform.new(%w[x86 darwin 8]), Gem::Platform.local
  end

  def test_self_match
    assert Gem::Platform.match(nil), 'nil == ruby'
    assert Gem::Platform.match(Gem::Platform.local), 'exact match'
    assert Gem::Platform.match(Gem::Platform.local.to_s), '=~ match'
    assert Gem::Platform.match(Gem::Platform::RUBY), 'ruby'
  end

  def test_self_new
    assert_equal Gem::Platform.local, Gem::Platform.new(Gem::Platform::CURRENT)
    assert_equal Gem::Platform::RUBY, Gem::Platform.new(Gem::Platform::RUBY)
    assert_equal Gem::Platform::RUBY, Gem::Platform.new(nil)
    assert_equal Gem::Platform::RUBY, Gem::Platform.new('')
  end

  def test_initialize
    test_cases = {
      'amd64-freebsd6'         => ['amd64',     'freebsd',   '6'],
      'hppa2.0w-hpux11.31'     => ['hppa2.0w',  'hpux',      '11'],
      'java'                   => [nil,         'java',      nil],
      'jruby'                  => [nil,         'java',      nil],
      'universal-dotnet'       => ['universal', 'dotnet',    nil],
      'universal-dotnet2.0'    => ['universal', 'dotnet',  '2.0'],
      'universal-dotnet4.0'    => ['universal', 'dotnet',  '4.0'],
      'powerpc-aix5.3.0.0'     => ['powerpc',   'aix',       '5'],
      'powerpc-darwin7'        => ['powerpc',   'darwin',    '7'],
      'powerpc-darwin8'        => ['powerpc',   'darwin',    '8'],
      'powerpc-linux'          => ['powerpc',   'linux',     nil],
      'powerpc64-linux'        => ['powerpc64', 'linux',     nil],
      'sparc-solaris2.10'      => ['sparc',     'solaris',   '2.10'],
      'sparc-solaris2.8'       => ['sparc',     'solaris',   '2.8'],
      'sparc-solaris2.9'       => ['sparc',     'solaris',   '2.9'],
      'universal-darwin8'      => ['universal', 'darwin',    '8'],
      'universal-darwin9'      => ['universal', 'darwin',    '9'],
      'i386-cygwin'            => ['x86',       'cygwin',    nil],
      'i686-darwin'            => ['x86',       'darwin',    nil],
      'i686-darwin8.4.1'       => ['x86',       'darwin',    '8'],
      'i386-freebsd4.11'       => ['x86',       'freebsd',   '4'],
      'i386-freebsd5'          => ['x86',       'freebsd',   '5'],
      'i386-freebsd6'          => ['x86',       'freebsd',   '6'],
      'i386-freebsd7'          => ['x86',       'freebsd',   '7'],
      'i386-java1.5'           => ['x86',       'java',      '1.5'],
      'x86-java1.6'            => ['x86',       'java',      '1.6'],
      'i386-java1.6'           => ['x86',       'java',      '1.6'],
      'i686-linux'             => ['x86',       'linux',     nil],
      'i586-linux'             => ['x86',       'linux',     nil],
      'i486-linux'             => ['x86',       'linux',     nil],
      'i386-linux'             => ['x86',       'linux',     nil],
      'i586-linux-gnu'         => ['x86',       'linux',     nil],
      'i386-linux-gnu'         => ['x86',       'linux',     nil],
      'i386-mingw32'           => ['x86',       'mingw32',   nil],
      'i386-mswin32'           => ['x86',       'mswin32',   nil],
      'i386-mswin32_80'        => ['x86',       'mswin32',   '80'],
      'i386-mswin32-80'        => ['x86',       'mswin32',   '80'],
      'x86-mswin32'            => ['x86',       'mswin32',   nil],
      'x86-mswin32_60'         => ['x86',       'mswin32',   '60'],
      'x86-mswin32-60'         => ['x86',       'mswin32',   '60'],
      'i386-netbsdelf'         => ['x86',       'netbsdelf', nil],
      'i386-openbsd4.0'        => ['x86',       'openbsd',   '4.0'],
      'i386-solaris2.10'       => ['x86',       'solaris',   '2.10'],
      'i386-solaris2.8'        => ['x86',       'solaris',   '2.8'],
      'mswin32'                => ['x86',       'mswin32',   nil],
      'x86_64-linux'           => ['x86_64',    'linux',     nil],
      'x86_64-openbsd3.9'      => ['x86_64',    'openbsd',   '3.9'],
      'x86_64-openbsd4.0'      => ['x86_64',    'openbsd',   '4.0'],
    }

    test_cases.each do |arch, expected|
      platform = Gem::Platform.new arch
      assert_equal expected, platform.to_a, arch.inspect
    end
  end

  def test_initialize_command_line
    expected = ['x86', 'mswin32', nil]

    platform = Gem::Platform.new 'i386-mswin32'

    assert_equal expected, platform.to_a, 'i386-mswin32'

    expected = ['x86', 'mswin32', '80']

    platform = Gem::Platform.new 'i386-mswin32-80'

    assert_equal expected, platform.to_a, 'i386-mswin32-80'

    expected = ['x86', 'solaris', '2.10']

    platform = Gem::Platform.new 'i386-solaris-2.10'

    assert_equal expected, platform.to_a, 'i386-solaris-2.10'
  end

  def test_initialize_mswin32_vc6
    orig_RUBY_SO_NAME = RbConfig::CONFIG['RUBY_SO_NAME']
    RbConfig::CONFIG['RUBY_SO_NAME'] = 'msvcrt-ruby18'

    expected = ['x86', 'mswin32', nil]

    platform = Gem::Platform.new 'i386-mswin32'

    assert_equal expected, platform.to_a, 'i386-mswin32 VC6'
  ensure
    RbConfig::CONFIG['RUBY_SO_NAME'] = orig_RUBY_SO_NAME
  end

  def test_initialize_platform
    platform = Gem::Platform.new 'cpu-my_platform1'

    assert_equal 'cpu', platform.cpu
    assert_equal 'my_platform', platform.os
    assert_equal '1', platform.version
  end

  def test_initialize_test
    platform = Gem::Platform.new 'cpu-my_platform1'
    assert_equal 'cpu', platform.cpu
    assert_equal 'my_platform', platform.os
    assert_equal '1', platform.version

    platform = Gem::Platform.new 'cpu-other_platform1'
    assert_equal 'cpu', platform.cpu
    assert_equal 'other_platform', platform.os
    assert_equal '1', platform.version
  end

  def test_empty
    platform = Gem::Platform.new 'cpu-other_platform1'
    assert_respond_to platform, :empty?
    assert_equal false, Gem::Deprecate.skip_during { platform.empty? }
  end

  def test_to_s
    if win_platform? then
      assert_equal 'x86-mswin32-60', Gem::Platform.local.to_s
    else
      assert_equal 'x86-darwin-8', Gem::Platform.local.to_s
    end
  end

  def test_equals2
    my = Gem::Platform.new %w[cpu my_platform 1]
    other = Gem::Platform.new %w[cpu other_platform 1]

    assert_equal my, my
    refute_equal my, other
    refute_equal other, my
  end

  def test_equals3
    my = Gem::Platform.new %w[cpu my_platform 1]
    other = Gem::Platform.new %w[cpu other_platform 1]

    assert(my === my)
    refute(other === my)
    refute(my === other)
  end

  def test_equals3_cpu
    ppc_darwin8 = Gem::Platform.new 'powerpc-darwin8.0'
    uni_darwin8 = Gem::Platform.new 'universal-darwin8.0'
    x86_darwin8 = Gem::Platform.new 'i686-darwin8.0'

    util_set_arch 'powerpc-darwin8'
    assert((ppc_darwin8 === Gem::Platform.local), 'powerpc =~ universal')
    assert((uni_darwin8 === Gem::Platform.local), 'powerpc =~ universal')
    refute((x86_darwin8 === Gem::Platform.local), 'powerpc =~ universal')

    util_set_arch 'i686-darwin8'
    refute((ppc_darwin8 === Gem::Platform.local), 'powerpc =~ universal')
    assert((uni_darwin8 === Gem::Platform.local), 'x86 =~ universal')
    assert((x86_darwin8 === Gem::Platform.local), 'powerpc =~ universal')

    util_set_arch 'universal-darwin8'
    assert((ppc_darwin8 === Gem::Platform.local), 'universal =~ ppc')
    assert((uni_darwin8 === Gem::Platform.local), 'universal =~ universal')
    assert((x86_darwin8 === Gem::Platform.local), 'universal =~ x86')
  end

  def test_equals3_version
    util_set_arch 'i686-darwin8'

    x86_darwin = Gem::Platform.new ['x86', 'darwin', nil]
    x86_darwin7 = Gem::Platform.new ['x86', 'darwin', '7']
    x86_darwin8 = Gem::Platform.new ['x86', 'darwin', '8']
    x86_darwin9 = Gem::Platform.new ['x86', 'darwin', '9']

    assert((x86_darwin  === Gem::Platform.local), 'x86_darwin === x86_darwin8')
    assert((x86_darwin8 === Gem::Platform.local), 'x86_darwin8 === x86_darwin8')

    refute((x86_darwin7 === Gem::Platform.local), 'x86_darwin7 === x86_darwin8')
    refute((x86_darwin9 === Gem::Platform.local), 'x86_darwin9 === x86_darwin8')
  end

  def test_equals_tilde
    util_set_arch 'i386-mswin32'

    assert_match 'mswin32',      Gem::Platform.local
    assert_match 'i386-mswin32', Gem::Platform.local

    # oddballs
    assert_match 'i386-mswin32-mq5.3', Gem::Platform.local
    assert_match 'i386-mswin32-mq6',   Gem::Platform.local
    refute_match 'win32-1.8.2-VC7',    Gem::Platform.local
    refute_match 'win32-1.8.4-VC6',    Gem::Platform.local
    refute_match 'win32-source',       Gem::Platform.local
    refute_match 'windows',            Gem::Platform.local

    util_set_arch 'i686-linux'
    assert_match 'i486-linux', Gem::Platform.local
    assert_match 'i586-linux', Gem::Platform.local
    assert_match 'i686-linux', Gem::Platform.local

    util_set_arch 'i686-darwin8'
    assert_match 'i686-darwin8.4.1', Gem::Platform.local
    assert_match 'i686-darwin8.8.2', Gem::Platform.local

    util_set_arch 'java'
    assert_match 'java',  Gem::Platform.local
    assert_match 'jruby', Gem::Platform.local

    util_set_arch 'universal-dotnet2.0'
    assert_match 'universal-dotnet',     Gem::Platform.local
    assert_match 'universal-dotnet-2.0', Gem::Platform.local
    refute_match 'universal-dotnet-4.0', Gem::Platform.local
    assert_match 'dotnet',               Gem::Platform.local
    assert_match 'dotnet-2.0',           Gem::Platform.local
    refute_match 'dotnet-4.0',           Gem::Platform.local

    util_set_arch 'universal-dotnet4.0'
    assert_match 'universal-dotnet',      Gem::Platform.local
    refute_match 'universal-dotnet-2.0',  Gem::Platform.local
    assert_match 'universal-dotnet-4.0',  Gem::Platform.local
    assert_match 'dotnet',                Gem::Platform.local
    refute_match 'dotnet-2.0',            Gem::Platform.local
    assert_match 'dotnet-4.0',            Gem::Platform.local

    util_set_arch 'powerpc-darwin'
    assert_match 'powerpc-darwin', Gem::Platform.local

    util_set_arch 'powerpc-darwin7'
    assert_match 'powerpc-darwin7.9.0', Gem::Platform.local

    util_set_arch 'powerpc-darwin8'
    assert_match 'powerpc-darwin8.10.0', Gem::Platform.local

    util_set_arch 'sparc-solaris2.8'
    assert_match 'sparc-solaris2.8-mq5.3', Gem::Platform.local
  end

end

