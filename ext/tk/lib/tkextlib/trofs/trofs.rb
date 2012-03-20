#
#  tkextlib/trofs/trofs.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/trofs/setup.rb'

# TkPackage.require('trofs', '0.4')
TkPackage.require('trofs')

module Tk
  module Trofs
    extend TkCore

    PACKAGE_NAME = 'trofs'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('trofs')
      rescue
        ''
      end
    end

    ##############################################

    def self.create_archive(dir, archive)
      tk_call('::trofs::archive', dir, archive)
      archive
    end

    def self.mount(archive, mountpoint=None)
      # returns the normalized path to mountpoint
      tk_call('::trofs::mount', archive, mountpoint)
    end

    def self.umount(mountpoint)
      tk_call('::trofs::umount', mountpoint)
      mountpoint
    end
  end
end
