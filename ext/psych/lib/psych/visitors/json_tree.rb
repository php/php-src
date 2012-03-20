require 'psych/json/ruby_events'

module Psych
  module Visitors
    class JSONTree < YAMLTree
      include Psych::JSON::RubyEvents

      def initialize options = {}, emitter = Psych::JSON::TreeBuilder.new
        super
      end

      def accept target
        if target.respond_to?(:encode_with)
          dump_coder target
        else
          send(@dispatch_cache[target.class], target)
        end
      end
    end
  end
end
