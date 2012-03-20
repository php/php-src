require 'rubygems/command'

##
# Installs RubyGems itself.  This command is ordinarily only available from a
# RubyGems checkout or tarball.

class Gem::Commands::SetupCommand < Gem::Command

  def initialize
    require 'tmpdir'

    super 'setup', 'Install RubyGems',
          :format_executable => true, :rdoc => true, :ri => true,
          :site_or_vendor => :sitelibdir,
          :destdir => '', :prefix => ''

    add_option '--prefix=PREFIX',
               'Prefix path for installing RubyGems',
               'Will not affect gem repository location' do |prefix, options|
      options[:prefix] = File.expand_path prefix
    end

    add_option '--destdir=DESTDIR',
               'Root directory to install RubyGems into',
               'Mainly used for packaging RubyGems' do |destdir, options|
      options[:destdir] = File.expand_path destdir
    end

    add_option '--[no-]vendor',
               'Install into vendorlibdir not sitelibdir' do |vendor, options|
      options[:site_or_vendor] = vendor ? :vendorlibdir : :sitelibdir
    end

    add_option '--[no-]format-executable',
               'Makes `gem` match ruby',
               'If ruby is ruby18, gem will be gem18' do |value, options|
      options[:format_executable] = value
    end

    add_option '--[no-]rdoc',
               'Generate RDoc documentation for RubyGems' do |value, options|
      options[:rdoc] = value
    end

    add_option '--[no-]ri',
               'Generate RI documentation for RubyGems' do |value, options|
      options[:ri] = value
    end
  end

  def check_ruby_version
    required_version = Gem::Requirement.new '>= 1.8.7'

    unless required_version.satisfied_by? Gem.ruby_version then
      alert_error "Expected Ruby version #{required_version}, is #{Gem.ruby_version}"
      terminate_interaction 1
    end
  end

  def defaults_str # :nodoc:
    "--format-executable --rdoc --ri"
  end

  def description # :nodoc:
    <<-EOF
Installs RubyGems itself.

RubyGems installs RDoc for itself in GEM_HOME.  By default this is:
  #{Gem.dir}

If you prefer a different directory, set the GEM_HOME environment variable.

RubyGems will install the gem command with a name matching ruby's
prefix and suffix.  If ruby was installed as `ruby18`, gem will be
installed as `gem18`.

By default, this RubyGems will install gem as:
  #{Gem.default_exec_format % 'gem'}
    EOF
  end

  def execute
    @verbose = Gem.configuration.really_verbose

    install_destdir = options[:destdir]

    unless install_destdir.empty? then
      ENV['GEM_HOME'] ||= File.join(install_destdir,
                                    Gem.default_dir.gsub(/^[a-zA-Z]:/, ''))
    end

    check_ruby_version

    require 'fileutils'
    if Gem.configuration.really_verbose then
      extend FileUtils::Verbose
    else
      extend FileUtils
    end

    lib_dir, bin_dir = make_destination_dirs install_destdir

    install_lib lib_dir

    install_executables bin_dir

    remove_old_bin_files bin_dir

    say "RubyGems #{Gem::VERSION} installed"

    uninstall_old_gemcutter

    install_rdoc

    say
    if @verbose then
      say "-" * 78
      say
    end

    release_notes = File.join Dir.pwd, 'History.txt'

    release_notes = if File.exist? release_notes then
                      open release_notes do |io|
                        text = io.gets '==='
                        text << io.gets('===')
                        text[0...-3].sub(/^# coding:.*?^=/m, '')
                      end
                    else
                      "Oh-no! Unable to find release notes!"
                    end

    say release_notes

    say
    say "-" * 78
    say

    say "RubyGems installed the following executables:"
    say @bin_file_names.map { |name| "\t#{name}\n" }
    say

    unless @bin_file_names.grep(/#{File::SEPARATOR}gem$/) then
      say "If `gem` was installed by a previous RubyGems installation, you may need"
      say "to remove it by hand."
      say
    end
  end

  def install_executables(bin_dir)
    say "Installing gem executable" if @verbose

    @bin_file_names = []

    Dir.chdir 'bin' do
      bin_files = Dir['*']

      bin_files.delete 'update_rubygems'

      bin_files.each do |bin_file|
        bin_file_formatted = if options[:format_executable] then
                               Gem.default_exec_format % bin_file
                             else
                               bin_file
                             end

        dest_file = File.join bin_dir, bin_file_formatted
        bin_tmp_file = File.join Dir.tmpdir, bin_file

        begin
          bin = File.readlines bin_file
          bin[0] = "#!#{Gem.ruby}\n"

          File.open bin_tmp_file, 'w' do |fp|
            fp.puts bin.join
          end

          install bin_tmp_file, dest_file, :mode => 0755
          @bin_file_names << dest_file
        ensure
          rm bin_tmp_file
        end

        next unless Gem.win_platform?

        begin
          bin_cmd_file = File.join Dir.tmpdir, "#{bin_file}.bat"

          File.open bin_cmd_file, 'w' do |file|
            file.puts <<-TEXT
@ECHO OFF
IF NOT "%~f0" == "~f0" GOTO :WinNT
@"#{File.basename(Gem.ruby).chomp('"')}" "#{dest_file}" %1 %2 %3 %4 %5 %6 %7 %8 %9
GOTO :EOF
:WinNT
@"#{File.basename(Gem.ruby).chomp('"')}" "%~dpn0" %*
TEXT
          end

          install bin_cmd_file, "#{dest_file}.bat", :mode => 0755
        ensure
          rm bin_cmd_file
        end
      end
    end
  end

  def install_lib(lib_dir)
    say "Installing RubyGems" if @verbose

    Dir.chdir 'lib' do
      lib_files = Dir[File.join('**', '*rb')]

      lib_files.each do |lib_file|
        dest_file = File.join lib_dir, lib_file
        dest_dir = File.dirname dest_file
        mkdir_p dest_dir unless File.directory? dest_dir

        install lib_file, dest_file, :mode => 0644
      end
    end
  end

  def install_rdoc
    gem_doc_dir = File.join Gem.dir, 'doc'
    rubygems_name = "rubygems-#{Gem::VERSION}"
    rubygems_doc_dir = File.join gem_doc_dir, rubygems_name

    if File.writable? gem_doc_dir and
       (not File.exist? rubygems_doc_dir or
        File.writable? rubygems_doc_dir) then
      say "Removing old RubyGems RDoc and ri" if @verbose
      Dir[File.join(Gem.dir, 'doc', 'rubygems-[0-9]*')].each do |dir|
        rm_rf dir
      end

      if options[:ri] then
        ri_dir = File.join rubygems_doc_dir, 'ri'
        say "Installing #{rubygems_name} ri into #{ri_dir}" if @verbose
        run_rdoc '--ri', '--op', ri_dir
      end

      if options[:rdoc] then
        rdoc_dir = File.join rubygems_doc_dir, 'rdoc'
        say "Installing #{rubygems_name} rdoc into #{rdoc_dir}" if @verbose
        run_rdoc '--op', rdoc_dir
      end
    elsif @verbose then
      say "Skipping RDoc generation, #{gem_doc_dir} not writable"
      say "Set the GEM_HOME environment variable if you want RDoc generated"
    end
  end

  def make_destination_dirs(install_destdir)
    lib_dir = nil
    bin_dir = nil

    prefix = options[:prefix]
    site_or_vendor = options[:site_or_vendor]

    if prefix.empty? then
      lib_dir = Gem::ConfigMap[site_or_vendor]
      bin_dir = Gem::ConfigMap[:bindir]
    else
      # Apple installed RubyGems into libdir, and RubyGems <= 1.1.0 gets
      # confused about installation location, so switch back to
      # sitelibdir/vendorlibdir.
      if defined?(APPLE_GEM_HOME) and
        # just in case Apple and RubyGems don't get this patched up proper.
        (prefix == Gem::ConfigMap[:libdir] or
         # this one is important
         prefix == File.join(Gem::ConfigMap[:libdir], 'ruby')) then
         lib_dir = Gem::ConfigMap[site_or_vendor]
         bin_dir = Gem::ConfigMap[:bindir]
      else
        lib_dir = File.join prefix, 'lib'
        bin_dir = File.join prefix, 'bin'
      end
    end

    unless install_destdir.empty? then
      lib_dir = File.join install_destdir, lib_dir.gsub(/^[a-zA-Z]:/, '')
      bin_dir = File.join install_destdir, bin_dir.gsub(/^[a-zA-Z]:/, '')
    end

    mkdir_p lib_dir
    mkdir_p bin_dir

    return lib_dir, bin_dir
  end

  def remove_old_bin_files(bin_dir)
    old_bin_files = {
      'gem_mirror' => 'gem mirror',
      'gem_server' => 'gem server',
      'gemlock' => 'gem lock',
      'gemri' => 'ri',
      'gemwhich' => 'gem which',
      'index_gem_repository.rb' => 'gem generate_index',
    }

    old_bin_files.each do |old_bin_file, new_name|
      old_bin_path = File.join bin_dir, old_bin_file
      next unless File.exist? old_bin_path

      deprecation_message = "`#{old_bin_file}` has been deprecated.  Use `#{new_name}` instead."

      File.open old_bin_path, 'w' do |fp|
        fp.write <<-EOF
#!#{Gem.ruby}

abort "#{deprecation_message}"
    EOF
      end

      next unless Gem.win_platform?

      File.open "#{old_bin_path}.bat", 'w' do |fp|
        fp.puts %{@ECHO.#{deprecation_message}}
      end
    end
  end

  def run_rdoc(*args)
    begin
      gem 'rdoc'
    rescue Gem::LoadError
    end

    require 'rdoc/rdoc'

    args << '--main' << 'README.rdoc' << '--quiet'
    args << '.'
    args << 'README.rdoc' << 'UPGRADING.rdoc'
    args << 'LICENSE.txt' << 'MIT.txt' << 'History.txt'

    r = RDoc::RDoc.new
    r.document args
  end

  def uninstall_old_gemcutter
    require 'rubygems/uninstaller'

    ui = Gem::Uninstaller.new('gemcutter', :all => true, :ignore => true,
                              :version => '< 0.4')
    ui.uninstall
  rescue Gem::InstallError
  end

end

