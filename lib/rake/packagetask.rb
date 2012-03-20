# Define a package task library to aid in the definition of
# redistributable package files.

require 'rake'
require 'rake/tasklib'

module Rake

  # Create a packaging task that will package the project into
  # distributable files (e.g zip archive or tar files).
  #
  # The PackageTask will create the following targets:
  #
  # [<b>:package</b>]
  #   Create all the requested package files.
  #
  # [<b>:clobber_package</b>]
  #   Delete all the package files.  This target is automatically
  #   added to the main clobber target.
  #
  # [<b>:repackage</b>]
  #   Rebuild the package files from scratch, even if they are not out
  #   of date.
  #
  # [<b>"<em>package_dir</em>/<em>name</em>-<em>version</em>.tgz"</b>]
  #   Create a gzipped tar package (if <em>need_tar</em> is true).
  #
  # [<b>"<em>package_dir</em>/<em>name</em>-<em>version</em>.tar.gz"</b>]
  #   Create a gzipped tar package (if <em>need_tar_gz</em> is true).
  #
  # [<b>"<em>package_dir</em>/<em>name</em>-<em>version</em>.tar.bz2"</b>]
  #   Create a bzip2'd tar package (if <em>need_tar_bz2</em> is true).
  #
  # [<b>"<em>package_dir</em>/<em>name</em>-<em>version</em>.zip"</b>]
  #   Create a zip package archive (if <em>need_zip</em> is true).
  #
  # Example:
  #
  #   Rake::PackageTask.new("rake", "1.2.3") do |p|
  #     p.need_tar = true
  #     p.package_files.include("lib/**/*.rb")
  #   end
  #
  class PackageTask < TaskLib
    # Name of the package (from the GEM Spec).
    attr_accessor :name

    # Version of the package (e.g. '1.3.2').
    attr_accessor :version

    # Directory used to store the package files (default is 'pkg').
    attr_accessor :package_dir

    # True if a gzipped tar file (tgz) should be produced (default is false).
    attr_accessor :need_tar

    # True if a gzipped tar file (tar.gz) should be produced (default is false).
    attr_accessor :need_tar_gz

    # True if a bzip2'd tar file (tar.bz2) should be produced (default is false).
    attr_accessor :need_tar_bz2

    # True if a zip file should be produced (default is false)
    attr_accessor :need_zip

    # List of files to be included in the package.
    attr_accessor :package_files

    # Tar command for gzipped or bzip2ed archives.  The default is 'tar'.
    attr_accessor :tar_command

    # Zip command for zipped archives.  The default is 'zip'.
    attr_accessor :zip_command

    # Create a Package Task with the given name and version.  Use +:noversion+
    # as the version to build a package without a version or to provide a
    # fully-versioned package name.

    def initialize(name=nil, version=nil)
      init(name, version)
      yield self if block_given?
      define unless name.nil?
    end

    # Initialization that bypasses the "yield self" and "define" step.
    def init(name, version)
      @name = name
      @version = version
      @package_files = Rake::FileList.new
      @package_dir = 'pkg'
      @need_tar = false
      @need_tar_gz = false
      @need_tar_bz2 = false
      @need_zip = false
      @tar_command = 'tar'
      @zip_command = 'zip'
    end

    # Create the tasks defined by this task library.
    def define
      fail "Version required (or :noversion)" if @version.nil?
      @version = nil if :noversion == @version

      desc "Build all the packages"
      task :package

      desc "Force a rebuild of the package files"
      task :repackage => [:clobber_package, :package]

      desc "Remove package products"
      task :clobber_package do
        rm_r package_dir rescue nil
      end

      task :clobber => [:clobber_package]

      [
        [need_tar, tgz_file, "z"],
        [need_tar_gz, tar_gz_file, "z"],
        [need_tar_bz2, tar_bz2_file, "j"]
      ].each do |(need, file, flag)|
        if need
          task :package => ["#{package_dir}/#{file}"]
          file "#{package_dir}/#{file}" => [package_dir_path] + package_files do
            chdir(package_dir) do
              sh %{#{@tar_command} #{flag}cvf #{file} #{package_name}}
            end
          end
        end
      end

      if need_zip
        task :package => ["#{package_dir}/#{zip_file}"]
        file "#{package_dir}/#{zip_file}" => [package_dir_path] + package_files do
          chdir(package_dir) do
            sh %{#{@zip_command} -r #{zip_file} #{package_name}}
          end
        end
      end

      directory package_dir

      file package_dir_path => @package_files do
        mkdir_p package_dir rescue nil
        @package_files.each do |fn|
          f = File.join(package_dir_path, fn)
          fdir = File.dirname(f)
          mkdir_p(fdir) if !File.exist?(fdir)
          if File.directory?(fn)
            mkdir_p(f)
          else
            rm_f f
            safe_ln(fn, f)
          end
        end
      end
      self
    end

    def package_name
      @version ? "#{@name}-#{@version}" : @name
    end

    def package_dir_path
      "#{package_dir}/#{package_name}"
    end

    def tgz_file
      "#{package_name}.tgz"
    end

    def tar_gz_file
      "#{package_name}.tar.gz"
    end

    def tar_bz2_file
      "#{package_name}.tar.bz2"
    end

    def zip_file
      "#{package_name}.zip"
    end
  end

end
