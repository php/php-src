#
# Copyright (c) 2008 James M. Lawrence
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

require 'rbconfig'

#
# Alternate implementations of system() and backticks `` on Windows
# for ruby-1.8 and earlier.
#
module Rake::AltSystem
  WINDOWS = RbConfig::CONFIG["host_os"] =~
    %r!(msdos|mswin|djgpp|mingw|[Ww]indows)!

  class << self
    def define_module_function(name, &block)
      define_method(name, &block)
      module_function(name)
    end
  end

  if WINDOWS and RUBY_VERSION < "1.9.0"
    RUNNABLE_EXTS = %w[com exe bat cmd]
    RUNNABLE_PATTERN = %r!\.(#{RUNNABLE_EXTS.join('|')})\Z!i

    define_module_function :kernel_system, &Kernel.method(:system)
    define_module_function :kernel_backticks, &Kernel.method(:'`')

    module_function

    def repair_command(cmd)
      "call " + (
        if cmd =~ %r!\A\s*\".*?\"!
          # already quoted
          cmd
        elsif match = cmd.match(%r!\A\s*(\S+)!)
          if match[1] =~ %r!/!
            # avoid x/y.bat interpretation as x with option /y
            %Q!"#{match[1]}"! + match.post_match
          else
            # a shell command will fail if quoted
            cmd
          end
        else
          # empty or whitespace
          cmd
        end
      )
    end

    def find_runnable(file)
      if file =~ RUNNABLE_PATTERN
        file
      else
        RUNNABLE_EXTS.each { |ext|
          if File.exist?(test = "#{file}.#{ext}")
            return test
          end
        }
        nil
      end
    end

    def system(cmd, *args)
      repaired = (
        if args.empty?
          [repair_command(cmd)]
        elsif runnable = find_runnable(cmd)
          [File.expand_path(runnable), *args]
        else
          # non-existent file
          [cmd, *args]
        end
      )
      kernel_system(*repaired)
    end

    def backticks(cmd)
      kernel_backticks(repair_command(cmd))
    end

    define_module_function :'`', &method(:backticks)
  else
    # Non-Windows or ruby-1.9+: same as Kernel versions
    define_module_function :system, &Kernel.method(:system)
    define_module_function :backticks, &Kernel.method(:'`')
    define_module_function :'`', &Kernel.method(:'`')
  end
end
