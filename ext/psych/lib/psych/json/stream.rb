require 'psych/json/ruby_events'
require 'psych/json/yaml_events'

module Psych
  module JSON
    class Stream < Psych::Visitors::JSONTree
      include Psych::JSON::RubyEvents
      include Psych::Streaming

      class Emitter < Psych::Stream::Emitter # :nodoc:
        include Psych::JSON::YAMLEvents
      end
    end
  end
end
