#!./miniruby -sI.

require './rbconfig'

CONFIG = RbConfig::MAKEFILE_CONFIG

version = RUBY_VERSION.split(/\./)
patch = CONFIG['PATCHLEVEL']
nversion = (version + [patch.to_i < 0 ? '0' : patch]).join(',')
sversion = version.join('.') + (patch.to_i < 0 ? 'dev' : "p#{patch}")

$ruby_name ||= CONFIG["RUBY_INSTALL_NAME"]
$rubyw_name ||= CONFIG["RUBYW_INSTALL_NAME"] || $ruby_name.sub(/ruby/, '\&w')
$so_name ||= CONFIG["RUBY_SO_NAME"]

icons = {}
def icons.find(path)
  if File.directory?(path)
    Dir.open(File.expand_path(path)) do |d|
      d.grep(/\.ico$/i) {|i| self[$`] = i}
    end
  else
    self[File.basename(path, '.ico')] = path
  end
  self
end

if ARGV.empty?
  icons.find('.')
else
  ARGV.each {|i| icons.find(i)}
end

ruby_icon = rubyw_icon = nil
[$ruby_name, 'ruby'].find do |i|
  if i = icons[i]
    ruby_icon = "1 ICON DISCARDABLE "+i.dump+"\n"
  end
end
[$rubyw_name, 'rubyw'].find do |i|
  if i = icons[i]
    rubyw_icon = "1 ICON DISCARDABLE "+i.dump+"\n"
  end
end
dll_icons = []
icons.keys.sort.each do |i|
  dll_icons << "#{dll_icons.size + 1} ICON DISCARDABLE "+icons[i].dump+"\n"
end

[ # base name    extension         file type  desc, icons
  [$ruby_name,   CONFIG["EXEEXT"], 'VFT_APP', 'CUI', ruby_icon],
  [$rubyw_name,  CONFIG["EXEEXT"], 'VFT_APP', 'GUI', rubyw_icon || ruby_icon],
  [$so_name,     '.dll',           'VFT_DLL', 'DLL', dll_icons.join],
].each do |base, ext, type, desc, icon|
  open(base + '.rc', "w") { |f|
    f.binmode if /mingw/ =~ RUBY_PLATFORM

    f.print <<EOF
#ifndef __BORLANDC__
#include <windows.h>
#include <winver.h>
#endif

#{icon || ''}
VS_VERSION_INFO VERSIONINFO
 FILEVERSION    #{nversion}
 PRODUCTVERSION #{nversion}
 FILEFLAGSMASK  0x3fL
 FILEFLAGS      0x0L
 FILEOS         VOS__WINDOWS32
 FILETYPE       #{type}
 FILESUBTYPE    VFT2_UNKNOWN
BEGIN
 BLOCK "StringFileInfo"
 BEGIN
  BLOCK "000004b0"
  BEGIN
   VALUE "Comments",         "#{RUBY_RELEASE_DATE}\\0"
   VALUE "CompanyName",      "http://www.ruby-lang.org/\\0"
   VALUE "FileDescription",  "Ruby interpreter (#{desc}) #{sversion} [#{RUBY_PLATFORM}]\\0"
   VALUE "FileVersion",      "#{sversion}\\0"
   VALUE "InternalName",     "#{base + ext}\\0"
   VALUE "LegalCopyright",   "Copyright (C) 1993-#{RUBY_RELEASE_DATE[/\d+/]} Yukihiro Matsumoto\\0"
   VALUE "OriginalFilename", "#{base + ext}\\0"
   VALUE "ProductName",      "Ruby interpreter #{sversion} [#{RUBY_PLATFORM}]\\0"
   VALUE "ProductVersion",   "#{sversion}\\0"
  END
 END
 BLOCK "VarFileInfo"
 BEGIN
  VALUE "Translation", 0x0, 0x4b0
 END
END
EOF
  }
end

