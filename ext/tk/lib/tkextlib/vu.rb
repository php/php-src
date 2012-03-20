#
#  The vu widget set support
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/vu/setup.rb'

# load package
# TkPackage.require('vu', '2.1')
TkPackage.require('vu')

# autoload
module Tk
  module Vu
    TkComm::TkExtlibAutoloadModule.unshift(self)

    PACKAGE_NAME = 'vu'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('vu')
      rescue
        ''
      end
    end

    ##########################################

    autoload :Dial,          'tkextlib/vu/dial'

    autoload :Pie,           'tkextlib/vu/pie'
    autoload :PieSlice,      'tkextlib/vu/pie'
    autoload :NamedPieSlice, 'tkextlib/vu/pie'

    autoload :Spinbox,       'tkextlib/vu/spinbox'

    autoload :Bargraph,      'tkextlib/vu/bargraph'
  end
end
