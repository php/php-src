require 'rubygems/command'
require 'rubygems/commands/query_command'

##
# An alternate to Gem::Commands::QueryCommand that searches for gems starting
# with the the supplied argument.

class Gem::Commands::ListCommand < Gem::Commands::QueryCommand

  def initialize
    super 'list', 'Display gems whose name starts with STRING'

    remove_option('--name-matches')
  end

  def arguments # :nodoc:
    "STRING        start of gem name to look for"
  end

  def defaults_str # :nodoc:
    "--local --no-details"
  end

  def usage # :nodoc:
    "#{program_name} [STRING]"
  end

  def execute
    string = get_one_optional_argument || ''
    options[:name] = /^#{string}/i
    super
  end

end

