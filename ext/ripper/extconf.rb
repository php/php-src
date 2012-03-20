#!ruby -s

require 'mkmf'
require 'rbconfig'

def main
  unless find_executable('bison')
    unless File.exist?('ripper.c') or File.exist?("#{$srcdir}/ripper.c")
      Logging.message 'missing bison; abort'
      return
    end
  end
  $objs = %w(ripper.o)
  $cleanfiles.concat %w(ripper.y ripper.c ripper.E ripper.output y.output eventids1.c eventids2table.c)
  $defs << '-DRIPPER'
  $defs << '-DRIPPER_DEBUG' if $debug
  $VPATH << '$(topdir)' << '$(top_srcdir)'
  $INCFLAGS << ' -I$(topdir) -I$(top_srcdir)'
  create_makefile 'ripper'
end

main
