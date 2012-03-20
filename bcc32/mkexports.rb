#!./miniruby -s

$:.unshift(File.expand_path("../..", __FILE__))
require 'win32/mkexports'

class Exports::Bcc < Exports
  def forwarding(internal, export)
    internal[/\A_?/]+export
  end

  def each_line(objs, &block)
    objs.each do |obj|
      opt = /\.(?:so|dll)\z/i =~ obj ? "-ee" : "-oiPUBDEF -oiPUBD32"
      IO.foreach("|tdump -q #{opt} #{obj.tr('/', '\\')} < nul", &block)
    end
  end

  def each_export(objs)
    objdump(objs) do |l|
      next unless /(?:PUBDEF|PUBD32|EXPORT)/ =~ l
      yield $1 if /'(.*?)'/ =~ l
    end
    yield "_strcasecmp", "_stricmp"
    yield "_strncasecmp", "_strnicmp"
  end
end
