#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

class Gem::Ext::Builder

  def self.class_name
    name =~ /Ext::(.*)Builder/
    $1.downcase
  end

  def self.make(dest_path, results)
    unless File.exist? 'Makefile' then
      raise Gem::InstallError, "Makefile not found:\n\n#{results.join "\n"}"
    end

    mf = File.read('Makefile')
    mf = mf.gsub(/^RUBYARCHDIR\s*=\s*\$[^$]*/, "RUBYARCHDIR = #{dest_path}")
    mf = mf.gsub(/^RUBYLIBDIR\s*=\s*\$[^$]*/, "RUBYLIBDIR = #{dest_path}")

    File.open('Makefile', 'wb') {|f| f.print mf}

    # try to find make program from Ruby configure arguments first
    RbConfig::CONFIG['configure_args'] =~ /with-make-prog\=(\w+)/
    make_program = $1 || ENV['make']
    unless make_program then
      make_program = (/mswin/ =~ RUBY_PLATFORM) ? 'nmake' : 'make'
    end

    ['', ' install'].each do |target|
      cmd = "#{make_program}#{target}"
      results << cmd
      results << `#{cmd} #{redirector}`

      raise Gem::InstallError, "make#{target} failed:\n\n#{results}" unless
        $?.success?
    end
  end

  def self.redirector
    '2>&1'
  end

  def self.run(command, results)
    results << command
    results << `#{command} #{redirector}`

    unless $?.success? then
      raise Gem::InstallError, "#{class_name} failed:\n\n#{results.join "\n"}"
    end
  end

end

