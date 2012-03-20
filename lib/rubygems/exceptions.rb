##
# Base exception class for RubyGems.  All exception raised by RubyGems are a
# subclass of this one.
class Gem::Exception < RuntimeError; end

class Gem::CommandLineError < Gem::Exception; end

class Gem::DependencyError < Gem::Exception; end

class Gem::DependencyRemovalException < Gem::Exception; end

##
# Raised when attempting to uninstall a gem that isn't in GEM_HOME.

class Gem::GemNotInHomeException < Gem::Exception
  attr_accessor :spec
end

class Gem::DocumentError < Gem::Exception; end

##
# Potentially raised when a specification is validated.
class Gem::EndOfYAMLException < Gem::Exception; end

##
# Signals that a file permission error is preventing the user from
# installing in the requested directories.
class Gem::FilePermissionError < Gem::Exception
  def initialize(path)
    super("You don't have write permissions into the #{path} directory.")
  end
end

##
# Used to raise parsing and loading errors
class Gem::FormatException < Gem::Exception
  attr_accessor :file_path
end

class Gem::GemNotFoundException < Gem::Exception
  def initialize(msg, name=nil, version=nil, errors=nil)
    super msg
    @name = name
    @version = version
    @errors = errors
  end

  attr_reader :name, :version, :errors
end

class Gem::InstallError < Gem::Exception; end

##
# Potentially raised when a specification is validated.
class Gem::InvalidSpecificationException < Gem::Exception; end

class Gem::OperationNotSupportedError < Gem::Exception; end

##
# Signals that a remote operation cannot be conducted, probably due to not
# being connected (or just not finding host).
#--
# TODO: create a method that tests connection to the preferred gems server.
# All code dealing with remote operations will want this.  Failure in that
# method should raise this error.
class Gem::RemoteError < Gem::Exception; end

class Gem::RemoteInstallationCancelled < Gem::Exception; end

class Gem::RemoteInstallationSkipped < Gem::Exception; end

##
# Represents an error communicating via HTTP.
class Gem::RemoteSourceException < Gem::Exception; end

class Gem::VerificationError < Gem::Exception; end

##
# Raised to indicate that a system exit should occur with the specified
# exit_code

class Gem::SystemExitException < SystemExit
  attr_accessor :exit_code

  def initialize(exit_code)
    @exit_code = exit_code

    super "Exiting RubyGems with exit_code #{exit_code}"
  end

end
