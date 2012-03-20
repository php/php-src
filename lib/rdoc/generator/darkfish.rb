# -*- mode: ruby; ruby-indent-level: 2; tab-width: 2 -*-

require 'pathname'
require 'fileutils'
require 'rdoc/erbio'

require 'rdoc/generator/markup'

##
# Darkfish RDoc HTML Generator
#
# $Id: darkfish.rb 52 2009-01-07 02:08:11Z deveiant $
#
# == Author/s
# * Michael Granger (ged@FaerieMUD.org)
#
# == Contributors
# * Mahlon E. Smith (mahlon@martini.nu)
# * Eric Hodel (drbrain@segment7.net)
#
# == License
#
# Copyright (c) 2007, 2008, Michael Granger. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the author/s, nor the names of the project's
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

class RDoc::Generator::Darkfish

  RDoc::RDoc.add_generator self

  include ERB::Util

  # Path to this file's parent directory. Used to find templates and other
  # resources.

  GENERATOR_DIR = File.join 'rdoc', 'generator'

  ##
  # Release Version

  VERSION = '2'

  ##
  # Description of this generator

  DESCRIPTION = 'HTML generator, written by Michael Granger'

  ##
  # Initialize a few instance variables before we start

  def initialize options
    @options = options

    @template_dir = Pathname.new options.template_dir
    @template_cache = {}

    @files      = nil
    @classes    = nil

    @basedir = Pathname.pwd.expand_path
  end

  ##
  # The output directory

  attr_reader :outputdir

  ##
  # Output progress information if debugging is enabled

  def debug_msg *msg
    return unless $DEBUG_RDOC
    $stderr.puts(*msg)
  end

  ##
  # Directory where generated class HTML files live relative to the output
  # dir.

  def class_dir
    nil
  end

  ##
  # Directory where generated class HTML files live relative to the output
  # dir.

  def file_dir
    nil
  end

  ##
  # Create the directories the generated docs will live in if they don't
  # already exist.

  def gen_sub_directories
    @outputdir.mkpath
  end

  ##
  # Copy over the stylesheet into the appropriate place in the output
  # directory.

  def write_style_sheet
    debug_msg "Copying static files"
    options = { :verbose => $DEBUG_RDOC, :noop => @options.dry_run }

    FileUtils.cp @template_dir + 'rdoc.css', '.', options

    Dir[(@template_dir + "{js,images}/**/*").to_s].each do |path|
      next if File.directory? path
      next if File.basename(path) =~ /^\./

        dst = Pathname.new(path).relative_path_from @template_dir

      # I suck at glob
      dst_dir = dst.dirname
      FileUtils.mkdir_p dst_dir, options unless File.exist? dst_dir

      FileUtils.cp @template_dir + path, dst, options
    end
  end

  ##
  # Build the initial indices and output objects based on an array of TopLevel
  # objects containing the extracted information.

  def generate top_levels
    @outputdir = Pathname.new(@options.op_dir).expand_path(@basedir)

    @files = top_levels.sort
    @classes = RDoc::TopLevel.all_classes_and_modules.sort
    @methods = @classes.map { |m| m.method_list }.flatten.sort
    @modsort = get_sorted_module_list(@classes)

    # Now actually write the output
    write_style_sheet
    generate_index
    generate_class_files
    generate_file_files

  rescue => e
    debug_msg "%s: %s\n  %s" % [
      e.class.name, e.message, e.backtrace.join("\n  ")
    ]

    raise
  end

  protected

  ##
  # Return a list of the documented modules sorted by salience first, then
  # by name.

  def get_sorted_module_list(classes)
    nscounts = classes.inject({}) do |counthash, klass|
      top_level = klass.full_name.gsub(/::.*/, '')
      counthash[top_level] ||= 0
      counthash[top_level] += 1

      counthash
    end

    # Sort based on how often the top level namespace occurs, and then on the
    # name of the module -- this works for projects that put their stuff into
    # a namespace, of course, but doesn't hurt if they don't.
    classes.sort_by do |klass|
      top_level = klass.full_name.gsub( /::.*/, '' )
      [nscounts[top_level] * -1, klass.full_name]
    end.select do |klass|
      klass.display?
    end
  end

  ##
  # Generate an index page which lists all the classes which are documented.

  def generate_index
    template_file = @template_dir + 'index.rhtml'
    return unless template_file.exist?

    debug_msg "Rendering the index page..."

    out_file = @basedir + @options.op_dir + 'index.html'

    render_template template_file, out_file do |io| binding end
  rescue => e
    error = RDoc::Error.new \
      "error generating index.html: #{e.message} (#{e.class})"
    error.set_backtrace e.backtrace

    raise error
  end

  ##
  # Generate a documentation file for each class

  def generate_class_files
    template_file = @template_dir + 'classpage.rhtml'
    return unless template_file.exist?
    debug_msg "Generating class documentation in #{@outputdir}"

    current = nil

    @classes.each do |klass|
      current = klass
      debug_msg "  working on %s (%s)" % [klass.full_name, klass.path]
      out_file   = @outputdir + klass.path
      # suppress 1.9.3 warning
      rel_prefix = rel_prefix = @outputdir.relative_path_from(out_file.dirname)
      svninfo    = svninfo    = self.get_svninfo(klass)

      debug_msg "  rendering #{out_file}"
      render_template template_file, out_file do |io| binding end
    end
  rescue => e
    error = RDoc::Error.new \
      "error generating #{current.path}: #{e.message} (#{e.class})"
    error.set_backtrace e.backtrace

    raise error
  end

  ##
  # Generate a documentation file for each file

  def generate_file_files
    template_file = @template_dir + 'filepage.rhtml'
    return unless template_file.exist?
    debug_msg "Generating file documentation in #{@outputdir}"

    out_file = nil

    @files.each do |file|
      out_file     = @outputdir + file.path
      debug_msg "  working on %s (%s)" % [file.full_name, out_file]
      # suppress 1.9.3 warning
      rel_prefix = rel_prefix  = @outputdir.relative_path_from(out_file.dirname)

      debug_msg "  rendering #{out_file}"
      render_template template_file, out_file do |io| binding end
    end
  rescue => e
    error =
      RDoc::Error.new "error generating #{out_file}: #{e.message} (#{e.class})"
    error.set_backtrace e.backtrace

    raise error
  end

  ##
  # Return a string describing the amount of time in the given number of
  # seconds in terms a human can understand easily.

  def time_delta_string seconds
    return 'less than a minute'          if seconds < 60
    return "#{seconds / 60} minute#{seconds / 60 == 1 ? '' : 's'}" if
                                            seconds < 3000     # 50 minutes
    return 'about one hour'              if seconds < 5400     # 90 minutes
    return "#{seconds / 3600} hours"     if seconds < 64800    # 18 hours
    return 'one day'                     if seconds < 86400    #  1 day
    return 'about one day'               if seconds < 172800   #  2 days
    return "#{seconds / 86400} days"     if seconds < 604800   #  1 week
    return 'about one week'              if seconds < 1209600  #  2 week
    return "#{seconds / 604800} weeks"   if seconds < 7257600  #  3 months
    return "#{seconds / 2419200} months" if seconds < 31536000 #  1 year
    return "#{seconds / 31536000} years"
  end

  # %q$Id: darkfish.rb 52 2009-01-07 02:08:11Z deveiant $"
  SVNID_PATTERN = /
    \$Id:\s
    (\S+)\s                # filename
    (\d+)\s                # rev
    (\d{4}-\d{2}-\d{2})\s  # Date (YYYY-MM-DD)
    (\d{2}:\d{2}:\d{2}Z)\s # Time (HH:MM:SSZ)
    (\w+)\s                # committer
    \$$
  /x

  ##
  # Try to extract Subversion information out of the first constant whose
  # value looks like a subversion Id tag. If no matching constant is found,
  # and empty hash is returned.

  def get_svninfo klass
    constants = klass.constants or return {}

    constants.find { |c| c.value =~ SVNID_PATTERN } or return {}

    filename, rev, date, time, committer = $~.captures
    commitdate = Time.parse "#{date} #{time}"

    return {
      :filename    => filename,
      :rev         => Integer(rev),
      :commitdate  => commitdate,
      :commitdelta => time_delta_string(Time.now - commitdate),
      :committer   => committer,
    }
  end

  ##
  # Load and render the erb template in the given +template_file+ and write
  # it out to +out_file+.
  #
  # Both +template_file+ and +out_file+ should be Pathname-like objects.
  #
  # An io will be yielded which must be captured by binding in the caller.

  def render_template template_file, out_file # :yield: io
    template = template_for template_file

    unless @options.dry_run then
      debug_msg "Outputting to %s" % [out_file.expand_path]

      out_file.dirname.mkpath
      out_file.open 'w', 0644 do |io|
        io.set_encoding @options.encoding if Object.const_defined? :Encoding

        context = yield io

        template_result template, context, template_file
      end
    else
      context = yield nil

      output = template_result template, context, template_file

      debug_msg "  would have written %d characters to %s" % [
        output.length, out_file.expand_path
      ]
    end
  end

  ##
  # Creates the result for +template+ with +context+.  If an error is raised a
  # Pathname +template_file+ will indicate the file where the error occurred.

  def template_result template, context, template_file
    template.filename = template_file.to_s
    template.result context
  rescue NoMethodError => e
    raise RDoc::Error, "Error while evaluating %s: %s" % [
      template_file.expand_path,
      e.message,
    ], e.backtrace
  end

  ##
  # Retrieves a cache template for +file+, if present, or fills the cache.

  def template_for file
    template = @template_cache[file]

    return template if template

    klass = @options.dry_run ? ERB : RDoc::ERBIO

    template = klass.new file.read, nil, '<>'
    @template_cache[file] = template
    template
  end

end

