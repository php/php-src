# -*- coding: utf-8 -*-
#--
# Copyright (C) 2004 Mauricio Julio Fernández Pradier
# See LICENSE.txt for additional licensing information.
#++

##
# TarOutput is a wrapper to TarWriter that builds gem-format tar file.
#
# Gem-format tar files contain the following files:
# [data.tar.gz] A gzipped tar file containing the files that compose the gem
#               which will be extracted into the gem/ dir on installation.
# [metadata.gz] A YAML format Gem::Specification.
# [data.tar.gz.sig] A signature for the gem's data.tar.gz.
# [metadata.gz.sig] A signature for the gem's metadata.gz.
#
# See TarOutput::open for usage details.

class Gem::Package::TarOutput

  ##
  # Creates a new TarOutput which will yield a TarWriter object for the
  # data.tar.gz portion of a gem-format tar file.
  #
  # See #initialize for details on +io+ and +signer+.
  #
  # See #add_gem_contents for details on adding metadata to the tar file.

  def self.open(io, signer = nil, &block) # :yield: data_tar_writer
    tar_outputter = new io, signer
    tar_outputter.add_gem_contents(&block)
    tar_outputter.add_metadata
    tar_outputter.add_signatures

  ensure
    tar_outputter.close
  end

  ##
  # Creates a new TarOutput that will write a gem-format tar file to +io+.  If
  # +signer+ is given, the data.tar.gz and metadata.gz will be signed and
  # the signatures will be added to the tar file.

  def initialize(io, signer)
    @io = io
    @signer = signer

    @tar_writer = Gem::Package::TarWriter.new @io

    @metadata = nil

    @data_signature = nil
    @meta_signature = nil
  end

  ##
  # Yields a TarWriter for the data.tar.gz inside a gem-format tar file.
  # The yielded TarWriter has been extended with a #metadata= method for
  # attaching a YAML format Gem::Specification which will be written by
  # add_metadata.

  def add_gem_contents
    @tar_writer.add_file "data.tar.gz", 0644 do |inner|
      sio = @signer ? StringIO.new : nil
      Zlib::GzipWriter.wrap(sio || inner) do |os|

        Gem::Package::TarWriter.new os do |data_tar_writer|
          # :stopdoc:
          def data_tar_writer.metadata() @metadata end
          def data_tar_writer.metadata=(metadata) @metadata = metadata end
          # :startdoc:

          yield data_tar_writer

          @metadata = data_tar_writer.metadata
        end
      end

      # if we have a signing key, then sign the data
      # digest and return the signature
      if @signer then
        require 'rubygems/security'
        digest = Gem::Security::OPT[:dgst_algo].digest sio.string
        @data_signature = @signer.sign digest
        inner.write sio.string
      end
    end

    self
  end

  ##
  # Adds metadata.gz to the gem-format tar file which was saved from a
  # previous #add_gem_contents call.

  def add_metadata
    return if @metadata.nil?

    @tar_writer.add_file "metadata.gz", 0644 do |io|
      begin
        sio = @signer ? StringIO.new : nil
        gzos = Zlib::GzipWriter.new(sio || io)
        gzos.write @metadata
      ensure
        gzos.flush
        gzos.finish

        # if we have a signing key, then sign the metadata digest and return
        # the signature
        if @signer then
          require 'rubygems/security'
          digest = Gem::Security::OPT[:dgst_algo].digest sio.string
          @meta_signature = @signer.sign digest
          io.write sio.string
        end
      end
    end
  end

  ##
  # Adds data.tar.gz.sig and metadata.gz.sig to the gem-format tar files if
  # a Gem::Security::Signer was sent to initialize.

  def add_signatures
    if @data_signature then
      @tar_writer.add_file 'data.tar.gz.sig', 0644 do |io|
        io.write @data_signature
      end
    end

    if @meta_signature then
      @tar_writer.add_file 'metadata.gz.sig', 0644 do |io|
        io.write @meta_signature
      end
    end
  end

  ##
  # Closes the TarOutput.

  def close
    @tar_writer.close
  end

end

