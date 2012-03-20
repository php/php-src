SIMPLE_GEM = <<-GEMDATA
        MD5SUM = "989bf34a1cbecd52e0ea66b662b3a405"
        if $0 == __FILE__
          require 'optparse'

          options = {}
          ARGV.options do |opts|
            opts.on_tail("--help", "show this message") {puts opts; exit}
            opts.on('--dir=DIRNAME', "Installation directory for the Gem") {|options[:directory]|}
            opts.on('--force', "Force Gem to install, bypassing dependency checks") {|options[:force]|}
            opts.on('--gen-rdoc', "Generate RDoc documentation for the Gem") {|options[:gen_rdoc]|}
            opts.parse!
          end

          require 'rubygems'
          @directory = options[:directory] || Gem.dir
          @force = options[:force]

          gem = Gem::Installer.new(__FILE__).install(@force, @directory)
          if options[:gen_rdoc]
            Gem::DocManager.new(gem).generate_rdoc
          end
end

__END__
--- !ruby/object:Gem::Specification
rubygems_version: "1.0"
name: testing
version: !ruby/object:Gem::Version
  version: 1.2.3
date: 2004-03-18 22:01:52.859121 -05:00
platform:
summary: This exercise the gem testing stuff.
require_paths:
  - lib
files:
  - lib/foo.rb
  - lib/test
  - lib/test.rb
  - lib/test/wow.rb
autorequire: test
test_suite_file: foo
requirements:
  - a computer processor
---
-
  size: 109
  mode: 420
  path: lib/foo.rb
-
  size: 0
  mode: 420
  path: lib/test.rb
-
  size: 15
  mode: 420
  path: lib/test/wow.rb
---
eJwVjDEKgDAUQ/eeIpsKguhY3ARPoHMp9quF0mL7e39/h5DwQpLpqz4TOqbC
U42eO6WuYEvBntIhECuaaX1KqXXLmy2kAEc32szExK+PjyBAlpTZyK0N/Twu
g1CKTjX9BGAj1w==
---
eJwDAAAAAAE=
---
eJwrKC0pVlAvzy9XyE3MU+cCACwiBP4=
GEMDATA
