#
# YAML::Syck module
# .. glues syck and yaml.rb together ..
#
require 'syck/basenode'

module Syck

  #
  # Mixin BaseNode functionality
  #
  class Node
    include Syck::BaseNode
  end

end
