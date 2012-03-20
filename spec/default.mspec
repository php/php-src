load File.dirname(__FILE__) + '/rubyspec/ruby.1.9.mspec'
class MSpecScript
  builddir = Dir.pwd
  srcdir = ENV['SRCDIR']
  if !srcdir and File.exist?("#{builddir}/Makefile") then 
    File.open("#{builddir}/Makefile", "r:US-ASCII") {|f|
      f.read[/^\s*srcdir\s*=\s*(.+)/i] and srcdir = $1
    }
  end
  config = proc{|name| `#{builddir}/miniruby -I#{srcdir} -r#{builddir}/rbconfig -e 'print RbConfig::CONFIG["#{name}"]'`}

  # The default implementation to run the specs.
  set :target, File.join(builddir, "miniruby#{config['exeext']}")
  set :prefix, File.expand_path('rubyspec', File.dirname(__FILE__))
  set :flags, %W[
    -I#{srcdir}/lib
    -I#{srcdir}/#{config['EXTOUT']}/common
    -I#{srcdir}/-
    #{srcdir}/tool/runruby.rb --extout=#{config['EXTOUT']}
  ]
end
