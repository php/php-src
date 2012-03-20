#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems/ext/builder'
require 'rubygems/command'

class Gem::Ext::ExtConfBuilder < Gem::Ext::Builder

  def self.build(extension, directory, dest_path, results)
    cmd = "#{Gem.ruby} #{File.basename extension}"
    cmd << " #{Gem::Command.build_args.join ' '}" unless Gem::Command.build_args.empty?

    run cmd, results

    make dest_path, results

    results
  end

end

