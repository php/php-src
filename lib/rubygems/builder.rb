#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems'
require 'rubygems/user_interaction'

Gem.load_yaml

require 'rubygems/package'

##
# The Builder class processes RubyGem specification files
# to produce a .gem file.

class Gem::Builder

  include Gem::UserInteraction

  ##
  # Constructs a builder instance for the provided specification
  #
  # spec:: [Gem::Specification] The specification instance

  def initialize(spec)
    @spec = spec
  end

  ##
  # Builds the gem from the specification.  Returns the name of the file
  # written.

  def build
    @spec.mark_version
    @spec.validate
    @signer = sign
    write_package
    say success if Gem.configuration.verbose
    File.basename @spec.cache_file
  end

  def success
    <<-EOM
  Successfully built RubyGem
  Name: #{@spec.name}
  Version: #{@spec.version}
  File: #{File.basename @spec.cache_file}
EOM
  end

  private

  ##
  # If the signing key was specified, then load the file, and swap to the
  # public key (TODO: we should probably just omit the signing key in favor of
  # the signing certificate, but that's for the future, also the signature
  # algorithm should be configurable)

  def sign
    signer = nil

    if @spec.respond_to?(:signing_key) and @spec.signing_key then
      require 'rubygems/security'

      signer = Gem::Security::Signer.new @spec.signing_key, @spec.cert_chain
      @spec.signing_key = nil
      @spec.cert_chain = signer.cert_chain.map { |cert| cert.to_s }
    end

    signer
  end

  def write_package
    file_name = File.basename @spec.cache_file
    open file_name, 'wb' do |gem_io|
      Gem::Package.open gem_io, 'w', @signer do |pkg|
        yaml = @spec.to_yaml
        pkg.metadata = yaml

        @spec.files.each do |file|
          next if File.directory?(file)
          next if file == file_name # Don't add gem onto itself

          stat = File.stat(file)
          mode = stat.mode & 0777
          size = stat.size

          pkg.add_file_simple file, mode, size do |tar_io|
            tar_io.write open(file, "rb") { |f| f.read }
          end
        end
      end
    end
  end

end

