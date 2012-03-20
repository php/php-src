######################################################################
# This file is imported from the minitest project.
# DO NOT make modifications in this repo. They _will_ be reverted!
# File a patch instead and assign it to Ryan Davis.
######################################################################

begin
  require 'rubygems'
  gem 'minitest'
rescue Gem::LoadError
  # do nothing
end

require 'minitest/unit'
require 'minitest/spec'
require 'minitest/mock'

MiniTest::Unit.autorun
