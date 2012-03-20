# rake/rdoctask is deprecated in favor of rdoc/task

if Rake.application
  Rake.application.deprecate('require \'rake/rdoctask\'', 'require \'rdoc/task\' (in RDoc 2.4.2+)', __FILE__)
end

require 'rubygems'

begin
  gem 'rdoc'
  require 'rdoc'
  require 'rdoc/task'
rescue LoadError, Gem::LoadError
end

# :stopdoc:

if defined?(RDoc::Task) then
  module Rake
    RDocTask = RDoc::Task unless const_defined? :RDocTask
  end
else
  require 'rake'
  require 'rake/tasklib'

  module Rake

    # NOTE: Rake::RDocTask is deprecated in favor of RDoc:Task which is included
    # in RDoc 2.4.2+.  Use require 'rdoc/task' to require it.
    #
    # Create a documentation task that will generate the RDoc files for
    # a project.
    #
    # The RDocTask will create the following targets:
    #
    # [<b><em>rdoc</em></b>]
    #   Main task for this RDOC task.
    #
    # [<b>:clobber_<em>rdoc</em></b>]
    #   Delete all the rdoc files.  This target is automatically
    #   added to the main clobber target.
    #
    # [<b>:re<em>rdoc</em></b>]
    #   Rebuild the rdoc files from scratch, even if they are not out
    #   of date.
    #
    # Simple Example:
    #
    #   Rake::RDocTask.new do |rd|
    #     rd.main = "README.rdoc"
    #     rd.rdoc_files.include("README.rdoc", "lib/**/*.rb")
    #   end
    #
    # The +rd+ object passed to the block is an RDocTask object. See the
    # attributes list for the RDocTask class for available customization options.
    #
    # == Specifying different task names
    #
    # You may wish to give the task a different name, such as if you are
    # generating two sets of documentation.  For instance, if you want to have a
    # development set of documentation including private methods:
    #
    #   Rake::RDocTask.new(:rdoc_dev) do |rd|
    #     rd.main = "README.doc"
    #     rd.rdoc_files.include("README.rdoc", "lib/**/*.rb")
    #     rd.options << "--all"
    #   end
    #
    # The tasks would then be named :<em>rdoc_dev</em>, :clobber_<em>rdoc_dev</em>, and
    # :re<em>rdoc_dev</em>.
    #
    # If you wish to have completely different task names, then pass a Hash as
    # first argument. With the <tt>:rdoc</tt>, <tt>:clobber_rdoc</tt> and
    # <tt>:rerdoc</tt> options, you can customize the task names to your liking.
    # For example:
    #
    #   Rake::RDocTask.new(:rdoc => "rdoc", :clobber_rdoc => "rdoc:clean", :rerdoc => "rdoc:force")
    #
    # This will create the tasks <tt>:rdoc</tt>, <tt>:rdoc_clean</tt> and
    # <tt>:rdoc:force</tt>.
    #
    class RDocTask < TaskLib
      # Name of the main, top level task.  (default is :rdoc)
      attr_accessor :name

      # Name of directory to receive the html output files. (default is "html")
      attr_accessor :rdoc_dir

      # Title of RDoc documentation. (defaults to rdoc's default)
      attr_accessor :title

      # Name of file to be used as the main, top level file of the
      # RDoc. (default is none)
      attr_accessor :main

      # Name of template to be used by rdoc. (defaults to rdoc's default)
      attr_accessor :template

      # List of files to be included in the rdoc generation. (default is [])
      attr_accessor :rdoc_files

      # Additional list of options to be passed rdoc.  (default is [])
      attr_accessor :options

      # Whether to run the rdoc process as an external shell (default is false)
      attr_accessor :external

      attr_accessor :inline_source

      # Create an RDoc task with the given name. See the RDocTask class overview
      # for documentation.
      def initialize(name = :rdoc)  # :yield: self
        if name.is_a?(Hash)
          invalid_options = name.keys.map { |k| k.to_sym } - [:rdoc, :clobber_rdoc, :rerdoc]
          if !invalid_options.empty?
            raise ArgumentError, "Invalid option(s) passed to RDocTask.new: #{invalid_options.join(", ")}"
          end
        end

        @name = name
        @rdoc_files = Rake::FileList.new
        @rdoc_dir = 'html'
        @main = nil
        @title = nil
        @template = nil
        @external = false
        @inline_source = true
        @options = []
        yield self if block_given?
        define
      end

      # Create the tasks defined by this task lib.
      def define
        if rdoc_task_name != "rdoc"
          desc "Build the RDOC HTML Files"
        else
          desc "Build the #{rdoc_task_name} HTML Files"
        end
        task rdoc_task_name

        desc "Force a rebuild of the RDOC files"
        task rerdoc_task_name => [clobber_task_name, rdoc_task_name]

        desc "Remove rdoc products"
        task clobber_task_name do
          rm_r rdoc_dir rescue nil
        end

        task :clobber => [clobber_task_name]

        directory @rdoc_dir
        task rdoc_task_name => [rdoc_target]
        file rdoc_target => @rdoc_files + [Rake.application.rakefile] do
          rm_r @rdoc_dir rescue nil
          @before_running_rdoc.call if @before_running_rdoc
          args = option_list + @rdoc_files
          if @external
            argstring = args.join(' ')
            sh %{ruby -Ivendor vendor/rd #{argstring}}
          else
            require 'rdoc/rdoc'
            RDoc::RDoc.new.document(args)
          end
        end
        self
      end

      def option_list
        result = @options.dup
        result << "-o" << @rdoc_dir
        result << "--main" << quote(main) if main
        result << "--title" << quote(title) if title
        result << "-T" << quote(template) if template
        result << "--inline-source" if inline_source && !@options.include?("--inline-source") && !@options.include?("-S")
        result
      end

      def quote(str)
        if @external
          "'#{str}'"
        else
          str
        end
      end

      def option_string
        option_list.join(' ')
      end

      # The block passed to this method will be called just before running the
      # RDoc generator. It is allowed to modify RDocTask attributes inside the
      # block.
      def before_running_rdoc(&block)
        @before_running_rdoc = block
      end

      private

      def rdoc_target
        "#{rdoc_dir}/index.html"
      end

      def rdoc_task_name
        case name
        when Hash
          (name[:rdoc] || "rdoc").to_s
        else
          name.to_s
        end
      end

      def clobber_task_name
        case name
        when Hash
          (name[:clobber_rdoc] || "clobber_rdoc").to_s
        else
          "clobber_#{name}"
        end
      end

      def rerdoc_task_name
        case name
        when Hash
          (name[:rerdoc] || "rerdoc").to_s
        else
          "re#{name}"
        end
      end

    end
  end
end

