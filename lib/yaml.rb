##
# The YAML module allows you to use one of the two YAML engines that ship with
# ruby.  By default Psych is used but the old and unmaintained Syck may be
# chosen.
#
# See Psych or Syck for usage and documentation.
#
# To set the YAML engine to syck:
#
#   YAML::ENGINE.yamler = 'syck'
#
# To set the YAML engine back to psych:
#
#   YAML::ENGINE.yamler = 'psych'

module YAML
  class EngineManager # :nodoc:
    attr_reader :yamler

    def initialize
      @yamler = nil
    end

    def syck?
      'syck' == @yamler
    end

    def yamler= engine
      raise(ArgumentError, "bad engine") unless %w{syck psych}.include?(engine)

      require engine unless (engine == 'syck' ? Syck : Psych).const_defined?(:VERSION)

      ::Object.class_eval <<-eorb, __FILE__, __LINE__ + 1
        remove_const 'YAML'
        YAML = #{engine.capitalize}
        remove_method :to_yaml
        alias :to_yaml :#{engine}_to_yaml
      eorb

      @yamler = engine
      engine
    end
  end

  ##
  # Allows changing the current YAML engine.  See YAML for details.

  ENGINE = YAML::EngineManager.new
end

if defined?(Psych)
  engine = 'psych'
elsif defined?(Syck)
  engine = 'syck'
else
  begin
    require 'psych'
    engine = 'psych'
  rescue LoadError
    warn "#{caller[0]}:"
    warn "It seems your ruby installation is missing psych (for YAML output)."
    warn "To eliminate this warning, please install libyaml and reinstall your ruby."
    require 'syck'
    engine = 'syck'
  end
end

module Syck
  ENGINE = YAML::ENGINE
end

module Psych
  ENGINE = YAML::ENGINE
end

YAML::ENGINE.yamler = engine
