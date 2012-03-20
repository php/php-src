require 'rubygems/command_manager'

##
# This is an example of exactly what NOT to do.
#
# DO NOT include code like this in your rubygems_plugin.rb

class Gem::Commands::InterruptCommand < Gem::Command

  def initialize
    super('interrupt', 'Raises an Interrupt Exception', {})
  end

  def execute
    raise Interrupt, "Interrupt exception"
  end

end

Gem::CommandManager.instance.register_command :interrupt

