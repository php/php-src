# -*- coding: iso-8859-1 -*-
#++
# Copyright (C) 2004 Mauricio Julio Fernández Pradier
# See LICENSE.txt for additional licensing information.
#--

require 'zlib'
Gem.load_yaml

class Gem::Package::TarInput

  include Gem::Package::FSyncDir
  include Enumerable

  attr_reader :metadata

  private_class_method :new

  def self.open(io, security_policy = nil,  &block)
    is = new io, security_policy

    yield is
  ensure
    is.close if is
  end

  def initialize(io, security_policy = nil)
    @io = io
    @tarreader = Gem::Package::TarReader.new @io
    has_meta = false

    data_sig, meta_sig, data_dgst, meta_dgst = nil, nil, nil, nil
    dgst_algo = security_policy ? Gem::Security::OPT[:dgst_algo] : nil

    @tarreader.each do |entry|
      case entry.full_name
      when "metadata"
        @metadata = load_gemspec entry.read
        has_meta = true
      when "metadata.gz"
        begin
          # if we have a security_policy, then pre-read the metadata file
          # and calculate it's digest
          sio = nil
          if security_policy
            Gem.ensure_ssl_available
            sio = StringIO.new(entry.read)
            meta_dgst = dgst_algo.digest(sio.string)
            sio.rewind
          end

          # Ruby 1.8 doesn't have encoding and YAML is UTF-8
          args = [sio || entry]
          args << { :external_encoding => Encoding::UTF_8 } if
            Object.const_defined?(:Encoding)

          gzis = Zlib::GzipReader.new(*args)

          # YAML wants an instance of IO
          @metadata = load_gemspec(gzis)
          has_meta = true
        ensure
          gzis.close unless gzis.nil?
        end
      when 'metadata.gz.sig'
        meta_sig = entry.read
      when 'data.tar.gz.sig'
        data_sig = entry.read
      when 'data.tar.gz'
        if security_policy
          Gem.ensure_ssl_available
          data_dgst = dgst_algo.digest(entry.read)
        end
      end
    end

    if security_policy then
      Gem.ensure_ssl_available

      # map trust policy from string to actual class (or a serialized YAML
      # file, if that exists)
      if String === security_policy then
        if Gem::Security::Policies.key? security_policy then
          # load one of the pre-defined security policies
          security_policy = Gem::Security::Policies[security_policy]
        elsif File.exist? security_policy then
          # FIXME: this doesn't work yet
          security_policy = YAML.load File.read(security_policy)
        else
          raise Gem::Exception, "Unknown trust policy '#{security_policy}'"
        end
      end

      if data_sig && data_dgst && meta_sig && meta_dgst then
        # the user has a trust policy, and we have a signed gem
        # file, so use the trust policy to verify the gem signature

        begin
          security_policy.verify_gem(data_sig, data_dgst, @metadata.cert_chain)
        rescue Exception => e
          raise "Couldn't verify data signature: #{e}"
        end

        begin
          security_policy.verify_gem(meta_sig, meta_dgst, @metadata.cert_chain)
        rescue Exception => e
          raise "Couldn't verify metadata signature: #{e}"
        end
      elsif security_policy.only_signed
        raise Gem::Exception, "Unsigned gem"
      else
        # FIXME: should display warning here (trust policy, but
        # either unsigned or badly signed gem file)
      end
    end

    @tarreader.rewind

    unless has_meta then
      path = io.path if io.respond_to? :path
      error = Gem::Package::FormatError.new 'no metadata found', path
      raise error
    end
  end

  def close
    @io.close
    @tarreader.close
  end

  def each(&block)
    @tarreader.each do |entry|
      next unless entry.full_name == "data.tar.gz"
      is = zipped_stream entry

      begin
        Gem::Package::TarReader.new is do |inner|
          inner.each(&block)
        end
      ensure
        is.close if is
      end
    end

    @tarreader.rewind
  end

  def extract_entry(destdir, entry, expected_md5sum = nil)
    if entry.directory? then
      dest = File.join destdir, entry.full_name

      if File.directory? dest then
        FileUtils.chmod entry.header.mode, dest, :verbose => false
      else
        FileUtils.mkdir_p dest, :mode => entry.header.mode, :verbose => false
      end

      fsync_dir dest
      fsync_dir File.join(dest, "..")

      return
    end

    # it's a file
    md5 = Digest::MD5.new if expected_md5sum
    destdir = File.join destdir, File.dirname(entry.full_name)
    FileUtils.mkdir_p destdir, :mode => 0755, :verbose => false
    destfile = File.join destdir, File.basename(entry.full_name)
    FileUtils.chmod 0600, destfile, :verbose => false rescue nil # Errno::ENOENT

    open destfile, "wb", entry.header.mode do |os|
      loop do
        data = entry.read 4096
        break unless data
        # HACK shouldn't we check the MD5 before writing to disk?
        md5 << data if expected_md5sum
        os.write(data)
      end

      os.fsync
    end

    FileUtils.chmod entry.header.mode, destfile, :verbose => false
    fsync_dir File.dirname(destfile)
    fsync_dir File.join(File.dirname(destfile), "..")

    if expected_md5sum && expected_md5sum != md5.hexdigest then
      raise Gem::Package::BadCheckSum
    end
  end

  # Attempt to YAML-load a gemspec from the given _io_ parameter.  Return
  # nil if it fails.
  def load_gemspec(io)
    Gem::Specification.from_yaml io
  rescue Gem::Exception
    nil
  end

  ##
  # Return an IO stream for the zipped entry.
  #
  # NOTE:  Originally this method used two approaches, Return a GZipReader
  # directly, or read the GZipReader into a string and return a StringIO on
  # the string.  The string IO approach was used for versions of ZLib before
  # 1.2.1 to avoid buffer errors on windows machines.  Then we found that
  # errors happened with 1.2.1 as well, so we changed the condition.  Then
  # we discovered errors occurred with versions as late as 1.2.3.  At this
  # point (after some benchmarking to show we weren't seriously crippling
  # the unpacking speed) we threw our hands in the air and declared that
  # this method would use the String IO approach on all platforms at all
  # times.  And that's the way it is.

  def zipped_stream(entry)
    if defined? Rubinius or defined? Maglev then
      # these implementations have working Zlib
      zis = Zlib::GzipReader.new entry
      dis = zis.read
      is = StringIO.new(dis)
    else
      # This is Jamis Buck's Zlib workaround for some unknown issue
      entry.read(10) # skip the gzip header
      zis = Zlib::Inflate.new(-Zlib::MAX_WBITS)
      is = StringIO.new(zis.inflate(entry.read))
    end
  ensure
    zis.finish if zis
  end

end

