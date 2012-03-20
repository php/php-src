require 'rubygems/command'

class Gem::Commands::HelpCommand < Gem::Command

  # :stopdoc:
  EXAMPLES = <<-EOF
Some examples of 'gem' usage.

* Install 'rake', either from local directory or remote server:

    gem install rake

* Install 'rake', only from remote server:

    gem install rake --remote

* Install 'rake', but only version 0.3.1, even if dependencies
  are not met, and into a user-specific directory:

    gem install rake --version 0.3.1 --force --user-install

* List local gems whose name begins with 'D':

    gem list D

* List local and remote gems whose name contains 'log':

    gem search log --both

* List only remote gems whose name contains 'log':

    gem search log --remote

* Uninstall 'rake':

    gem uninstall rake

* Create a gem:

    See http://rubygems.rubyforge.org/wiki/wiki.pl?CreateAGemInTenMinutes

* See information about RubyGems:

    gem environment

* Update all gems on your system:

    gem update
  EOF

  PLATFORMS = <<-'EOF'
RubyGems platforms are composed of three parts, a CPU, an OS, and a
version.  These values are taken from values in rbconfig.rb.  You can view
your current platform by running `gem environment`.

RubyGems matches platforms as follows:

  * The CPU must match exactly, unless one of the platforms has
    "universal" as the CPU.
  * The OS must match exactly.
  * The versions must match exactly unless one of the versions is nil.

For commands that install, uninstall and list gems, you can override what
RubyGems thinks your platform is with the --platform option.  The platform
you pass must match "#{cpu}-#{os}" or "#{cpu}-#{os}-#{version}".  On mswin
platforms, the version is the compiler version, not the OS version.  (Ruby
compiled with VC6 uses "60" as the compiler version, VC8 uses "80".)

Example platforms:

  x86-freebsd        # Any FreeBSD version on an x86 CPU
  universal-darwin-8 # Darwin 8 only gems that run on any CPU
  x86-mswin32-80     # Windows gems compiled with VC8

When building platform gems, set the platform in the gem specification to
Gem::Platform::CURRENT.  This will correctly mark the gem with your ruby's
platform.
  EOF
  # :startdoc:

  def initialize
    super 'help', "Provide help on the 'gem' command"
  end

  def arguments # :nodoc:
    args = <<-EOF
      commands      List all 'gem' commands
      examples      Show examples of 'gem' usage
      <command>     Show specific help for <command>
    EOF
    return args.gsub(/^\s+/, '')
  end

  def usage # :nodoc:
    "#{program_name} ARGUMENT"
  end

  def execute
    command_manager = Gem::CommandManager.instance
    arg = options[:args][0]

    if begins? "commands", arg then
      out = []
      out << "GEM commands are:"
      out << nil

      margin_width = 4

      desc_width = command_manager.command_names.map { |n| n.size }.max + 4

      summary_width = 80 - margin_width - desc_width
      wrap_indent = ' ' * (margin_width + desc_width)
      format = "#{' ' * margin_width}%-#{desc_width}s%s"

      command_manager.command_names.each do |cmd_name|
        summary = command_manager[cmd_name].summary
        summary = wrap(summary, summary_width).split "\n"
        out << sprintf(format, cmd_name, summary.shift)
        until summary.empty? do
          out << "#{wrap_indent}#{summary.shift}"
        end
      end

      out << nil
      out << "For help on a particular command, use 'gem help COMMAND'."
      out << nil
      out << "Commands may be abbreviated, so long as they are unambiguous."
      out << "e.g. 'gem i rake' is short for 'gem install rake'."

      say out.join("\n")

    elsif begins? "options", arg then
      say Gem::Command::HELP

    elsif begins? "examples", arg then
      say EXAMPLES

    elsif begins? "platforms", arg then
      say PLATFORMS

    elsif options[:help] then
      command = command_manager[options[:help]]
      if command
        # help with provided command
        command.invoke("--help")
      else
        alert_error "Unknown command #{options[:help]}.  Try 'gem help commands'"
      end

    elsif arg then
      possibilities = command_manager.find_command_possibilities(arg.downcase)
      if possibilities.size == 1
        command = command_manager[possibilities.first]
        command.invoke("--help")
      elsif possibilities.size > 1
        alert_warning "Ambiguous command #{arg} (#{possibilities.join(', ')})"
      else
        alert_warning "Unknown command #{arg}. Try gem help commands"
      end

    else
      say Gem::Command::HELP
    end
  end

end

