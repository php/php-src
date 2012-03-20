#
# = un.rb
#
# Copyright (c) 2003 WATANABE Hirofumi <eban@ruby-lang.org>
#
# This program is free software.
# You can distribute/modify this program under the same terms of Ruby.
#
# == Utilities to replace common UNIX commands in Makefiles etc
#
# == SYNOPSIS
#
#   ruby -run -e cp -- [OPTION] SOURCE DEST
#   ruby -run -e ln -- [OPTION] TARGET LINK_NAME
#   ruby -run -e mv -- [OPTION] SOURCE DEST
#   ruby -run -e rm -- [OPTION] FILE
#   ruby -run -e mkdir -- [OPTION] DIRS
#   ruby -run -e rmdir -- [OPTION] DIRS
#   ruby -run -e install -- [OPTION] SOURCE DEST
#   ruby -run -e chmod -- [OPTION] OCTAL-MODE FILE
#   ruby -run -e touch -- [OPTION] FILE
#   ruby -run -e wait_writable -- [OPTION] FILE
#   ruby -run -e mkmf -- [OPTION] EXTNAME [OPTION]
#   ruby -run -e httpd -- [OPTION] DocumentRoot
#   ruby -run -e help [COMMAND]

require "fileutils"
require "optparse"

module FileUtils
#  @fileutils_label = ""
  @fileutils_output = $stdout
end

def setup(options = "", *long_options)
  opt_hash = {}
  argv = []
  OptionParser.new do |o|
    options.scan(/.:?/) do |s|
      opt_name = s.delete(":").intern
      o.on("-" + s.tr(":", " ")) do |val|
        opt_hash[opt_name] = val
      end
    end
    long_options.each do |s|
      opt_name, arg_name = s.split(/(?=[\s=])/, 2)
      opt_name.sub!(/\A--/, '')
      s = "--#{opt_name.gsub(/([A-Z]+|[a-z])([A-Z])/, '\1-\2').downcase}#{arg_name}"
      puts "#{opt_name}=>#{s}" if $DEBUG
      opt_name = opt_name.intern
      o.on(s) do |val|
        opt_hash[opt_name] = val
      end
    end
    o.on("-v") do opt_hash[:verbose] = true end
    o.order!(ARGV) do |x|
      if /[*?\[{]/ =~ x
        argv.concat(Dir[x])
      else
        argv << x
      end
    end
  end
  yield argv, opt_hash
end

##
# Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY
#
#   ruby -run -e cp -- [OPTION] SOURCE DEST
#
#   -p          preserve file attributes if possible
#   -r          copy recursively
#   -v          verbose
#

def cp
  setup("pr") do |argv, options|
    cmd = "cp"
    cmd += "_r" if options.delete :r
    options[:preserve] = true if options.delete :p
    dest = argv.pop
    argv = argv[0] if argv.size == 1
    FileUtils.send cmd, argv, dest, options
  end
end

##
# Create a link to the specified TARGET with LINK_NAME.
#
#   ruby -run -e ln -- [OPTION] TARGET LINK_NAME
#
#   -s          make symbolic links instead of hard links
#   -f          remove existing destination files
#   -v          verbose
#

def ln
  setup("sf") do |argv, options|
    cmd = "ln"
    cmd += "_s" if options.delete :s
    options[:force] = true if options.delete :f
    dest = argv.pop
    argv = argv[0] if argv.size == 1
    FileUtils.send cmd, argv, dest, options
  end
end

##
# Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.
#
#   ruby -run -e mv -- [OPTION] SOURCE DEST
#
#   -v          verbose
#

def mv
  setup do |argv, options|
    dest = argv.pop
    argv = argv[0] if argv.size == 1
    FileUtils.mv argv, dest, options
  end
end

##
# Remove the FILE
#
#   ruby -run -e rm -- [OPTION] FILE
#
#   -f          ignore nonexistent files
#   -r          remove the contents of directories recursively
#   -v          verbose
#

def rm
  setup("fr") do |argv, options|
    cmd = "rm"
    cmd += "_r" if options.delete :r
    options[:force] = true if options.delete :f
    FileUtils.send cmd, argv, options
  end
end

##
# Create the DIR, if they do not already exist.
#
#   ruby -run -e mkdir -- [OPTION] DIR
#
#   -p          no error if existing, make parent directories as needed
#   -v          verbose
#

def mkdir
  setup("p") do |argv, options|
    cmd = "mkdir"
    cmd += "_p" if options.delete :p
    FileUtils.send cmd, argv, options
  end
end

##
# Remove the DIR.
#
#   ruby -run -e rmdir -- [OPTION] DIR
#
#   -p          remove DIRECTORY and its ancestors.
#   -v          verbose
#

def rmdir
  setup("p") do |argv, options|
    options[:parents] = true if options.delete :p
    FileUtils.rmdir argv, options
  end
end

##
# Copy SOURCE to DEST.
#
#   ruby -run -e install -- [OPTION] SOURCE DEST
#
#   -p          apply access/modification times of SOURCE files to
#               corresponding destination files
#   -m          set permission mode (as in chmod), instead of 0755
#   -v          verbose
#

def install
  setup("pm:") do |argv, options|
    options[:mode] = (mode = options.delete :m) ? mode.oct : 0755
    options[:preserve] = true if options.delete :p
    dest = argv.pop
    argv = argv[0] if argv.size == 1
    FileUtils.install argv, dest, options
  end
end

##
# Change the mode of each FILE to OCTAL-MODE.
#
#   ruby -run -e chmod -- [OPTION] OCTAL-MODE FILE
#
#   -v          verbose
#

def chmod
  setup do |argv, options|
    mode = argv.shift.oct
    FileUtils.chmod mode, argv, options
  end
end

##
# Update the access and modification times of each FILE to the current time.
#
#   ruby -run -e touch -- [OPTION] FILE
#
#   -v          verbose
#

def touch
  setup do |argv, options|
    FileUtils.touch argv, options
  end
end

##
# Wait until the file becomes writable.
#
#   ruby -run -e wait_writable -- [OPTION] FILE
#
#   -n RETRY    count to retry
#   -w SEC      each wait time in seconds
#   -v          verbose
#

def wait_writable
  setup("n:w:v") do |argv, options|
    verbose = options[:verbose]
    n = options[:n] and n = Integer(n)
    wait = (wait = options[:w]) ? Float(wait) : 0.2
    argv.each do |file|
      begin
        open(file, "r+b")
      rescue Errno::ENOENT
        break
      rescue Errno::EACCES => e
        raise if n and (n -= 1) <= 0
        puts e
        STDOUT.flush
        sleep wait
        retry
      end
    end
  end
end

##
# Create makefile using mkmf.
#
#   ruby -run -e mkmf -- [OPTION] EXTNAME [OPTION]
#
#   -d ARGS     run dir_config
#   -h ARGS     run have_header
#   -l ARGS     run have_library
#   -f ARGS     run have_func
#   -v ARGS     run have_var
#   -t ARGS     run have_type
#   -m ARGS     run have_macro
#   -c ARGS     run have_const
#   --vendor    install to vendor_ruby
#

def mkmf
  setup("d:h:l:f:v:t:m:c:", "vendor") do |argv, options|
    require 'mkmf'
    opt = options[:d] and opt.split(/:/).each {|n| dir_config(*n.split(/,/))}
    opt = options[:h] and opt.split(/:/).each {|n| have_header(*n.split(/,/))}
    opt = options[:l] and opt.split(/:/).each {|n| have_library(*n.split(/,/))}
    opt = options[:f] and opt.split(/:/).each {|n| have_func(*n.split(/,/))}
    opt = options[:v] and opt.split(/:/).each {|n| have_var(*n.split(/,/))}
    opt = options[:t] and opt.split(/:/).each {|n| have_type(*n.split(/,/))}
    opt = options[:m] and opt.split(/:/).each {|n| have_macro(*n.split(/,/))}
    opt = options[:c] and opt.split(/:/).each {|n| have_const(*n.split(/,/))}
    $configure_args["--vendor"] = true if options[:vendor]
    create_makefile(*argv)
  end
end

##
# Run WEBrick HTTP server.
#
#   ruby -run -e httpd -- [OPTION] DocumentRoot
#
#   --bind-address=ADDR         address to bind
#   --port=NUM                  listening port number
#   --max-clients=MAX           max number of simultaneous clients
#   --temp-dir=DIR              temporary directory
#   --do-not-reverse-lookup     disable reverse lookup
#   --request-timeout=SECOND    request timeout in seconds
#   --http-version=VERSION      HTTP version
#   -v                          verbose
#

def httpd
  setup("", "BindAddress=ADDR", "Port=PORT", "MaxClients=NUM", "TempDir=DIR",
        "DoNotReverseLookup", "RequestTimeout=SECOND", "HTTPVersion=VERSION") do
    |argv, options|
    require 'webrick'
    opt = options[:RequestTimeout] and options[:RequestTimeout] = opt.to_i
    [:Port, :MaxClients].each do |name|
      opt = options[name] and (options[name] = Integer(opt)) rescue nil
    end
    unless argv.empty?
      options[:DocumentRoot] = argv.shift
    end
    s = WEBrick::HTTPServer.new(options)
    shut = proc {s.shutdown}
    Signal.trap("TERM", shut)
    Signal.trap("QUIT", shut) if Signal.list.has_key?("QUIT")
    if STDIN.tty?
      Signal.trap("HUP", shut) if Signal.list.has_key?("HUP")
      Signal.trap("INT", shut)
    end
    s.start
  end
end

##
# Display help message.
#
#   ruby -run -e help [COMMAND]
#

def help
  setup do |argv,|
    all = argv.empty?
    open(__FILE__) do |me|
      while me.gets("##\n")
        if help = me.gets("\n\n")
          if all or argv.delete help[/-e \w+/].sub(/-e /, "")
            print help.gsub(/^# ?/, "")
          end
        end
      end
    end
  end
end
