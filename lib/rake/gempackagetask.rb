# rake/gempackagetask is deprecated in favor of rubygems/package_task

warn 'rake/gempackagetask is deprecated.  Use rubygems/package_task instead'

require 'rubygems'
require 'rubygems/package_task'

require 'rake'

# :stopdoc:

module Rake
  GemPackageTask = Gem::PackageTask
end

