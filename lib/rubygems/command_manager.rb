#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems/command'
require 'rubygems/user_interaction'

##
# The command manager registers and installs all the individual sub-commands
# supported by the gem command.
#
# Extra commands can be provided by writing a rubygems_plugin.rb
# file in an installed gem.  You should register your command against the
# Gem::CommandManager instance, like this:
#
#   # file rubygems_plugin.rb
#   require 'rubygems/command_manager'
#
#   class Gem::Commands::EditCommand < Gem::Command
#     # ...
#   end
#
#   Gem::CommandManager.instance.register_command :edit
#
# See Gem::Command for instructions on writing gem commands.

class Gem::CommandManager

  include Gem::UserInteraction

  ##
  # Return the authoritative instance of the command manager.

  def self.instance
    @command_manager ||= new
  end

  ##
  # Reset the authoritative instance of the command manager.

  def self.reset
    @command_manager = nil
  end

  ##
  # Register all the subcommands supported by the gem command.

  def initialize
    require 'timeout'
    @commands = {}
    register_command :build
    register_command :cert
    register_command :check
    register_command :cleanup
    register_command :contents
    register_command :dependency
    register_command :environment
    register_command :fetch
    register_command :generate_index
    register_command :help
    register_command :install
    register_command :list
    register_command :lock
    register_command :outdated
    register_command :owner
    register_command :pristine
    register_command :push
    register_command :query
    register_command :rdoc
    register_command :search
    register_command :server
    register_command :sources
    register_command :specification
    register_command :stale
    register_command :uninstall
    register_command :unpack
    register_command :update
    register_command :which
  end

  ##
  # Register the Symbol +command+ as a gem command.

  def register_command(command)
    @commands[command] = false
  end

  ##
  # Unregister the Symbol +command+ as a gem command.

  def unregister_command(command)
    @commands.delete command
  end

  ##
  # Return the registered command from the command name.

  def [](command_name)
    command_name = command_name.intern
    return nil if @commands[command_name].nil?
    @commands[command_name] ||= load_and_instantiate(command_name)
  end

  ##
  # Return a sorted list of all command names (as strings).

  def command_names
    @commands.keys.collect {|key| key.to_s}.sort
  end

  ##
  # Run the config specified by +args+.

  def run(args)
    process_args(args)
  rescue StandardError, Timeout::Error => ex
    alert_error "While executing gem ... (#{ex.class})\n    #{ex.to_s}"
    ui.errs.puts "\t#{ex.backtrace.join "\n\t"}" if
      Gem.configuration.backtrace
    terminate_interaction(1)
  rescue Interrupt
    alert_error "Interrupted"
    terminate_interaction(1)
  end

  def process_args(args)
    args = args.to_str.split(/\s+/) if args.respond_to?(:to_str)
    if args.size == 0
      say Gem::Command::HELP
      terminate_interaction(1)
    end
    case args[0]
    when '-h', '--help'
      say Gem::Command::HELP
      terminate_interaction(0)
    when '-v', '--version'
      say Gem::VERSION
      terminate_interaction(0)
    when /^-/
      alert_error "Invalid option: #{args[0]}.  See 'gem --help'."
      terminate_interaction(1)
    else
      cmd_name = args.shift.downcase
      cmd = find_command(cmd_name)
      cmd.invoke(*args)
    end
  end

  def find_command(cmd_name)
    possibilities = find_command_possibilities cmd_name
    if possibilities.size > 1 then
      raise "Ambiguous command #{cmd_name} matches [#{possibilities.join(', ')}]"
    elsif possibilities.size < 1 then
      raise "Unknown command #{cmd_name}"
    end

    self[possibilities.first]
  end

  def find_command_possibilities(cmd_name)
    len = cmd_name.length

    command_names.select { |n| cmd_name == n[0, len] }
  end

  private

  def load_and_instantiate(command_name)
    command_name = command_name.to_s
    const_name = command_name.capitalize.gsub(/_(.)/) { $1.upcase } << "Command"
    commands = Gem::Commands
    retried = false

    begin
      commands.const_get(const_name).new
    rescue NameError
      raise if retried

      retried = true
      begin
        require "rubygems/commands/#{command_name}_command"
      rescue Exception => e
        alert_error "Loading command: #{command_name} (#{e.class})\n    #{e}"
        ui.errs.puts "\t#{e.backtrace.join "\n\t"}" if
          Gem.configuration.backtrace
      end
      retry
    end
  end

end

