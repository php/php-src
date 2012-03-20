#!/usr/bin/env ruby

require 'openssl'
require 'digest/md5'

class CHashDir
  include Enumerable

  def initialize(dirpath)
    @dirpath = dirpath
    @fingerprint_cache = @cert_cache = @crl_cache = nil
  end

  def hash_dir(silent = false)
    # ToDo: Should lock the directory...
    @silent = silent
    @fingerprint_cache = Hash.new
    @cert_cache = Hash.new
    @crl_cache = Hash.new
    do_hash_dir
  end

  def get_certs(name = nil)
    if name
      @cert_cache[hash_name(name)]
    else
      @cert_cache.values.flatten
    end
  end

  def get_crls(name = nil)
    if name
      @crl_cache[hash_name(name)]
    else
      @crl_cache.values.flatten
    end
  end

  def delete_crl(crl)
    File.unlink(crl_filename(crl))
    hash_dir(true)
  end

  def add_crl(crl)
    File.open(crl_filename(crl), "w") do |f|
      f << crl.to_pem
    end
    hash_dir(true)
  end

  def load_pem_file(filepath)
    str = File.read(filepath)
    begin
      OpenSSL::X509::Certificate.new(str)
    rescue
      begin
	OpenSSL::X509::CRL.new(str)
      rescue
	begin
	  OpenSSL::X509::Request.new(str)
	rescue
	  nil
	end
      end
    end
  end

private

  def crl_filename(crl)
    path(hash_name(crl.issuer)) + '.pem'
  end

  def do_hash_dir
    Dir.chdir(@dirpath) do
      delete_symlink
      Dir.glob('*.pem') do |pemfile|
	cert = load_pem_file(pemfile)
	case cert
	when OpenSSL::X509::Certificate
	  link_hash_cert(pemfile, cert)
	when OpenSSL::X509::CRL
	  link_hash_crl(pemfile, cert)
	else
	  STDERR.puts("WARNING: #{pemfile} does not contain a certificate or CRL: skipping") unless @silent
	end
      end
    end
  end

  def delete_symlink
    Dir.entries(".").each do |entry|
      next unless /^[\da-f]+\.r{0,1}\d+$/ =~ entry
      File.unlink(entry) if FileTest.symlink?(entry)
    end
  end

  def link_hash_cert(org_filename, cert)
    name_hash = hash_name(cert.subject)
    fingerprint = fingerprint(cert.to_der)
    filepath = link_hash(org_filename, name_hash, fingerprint) { |idx|
      "#{name_hash}.#{idx}"
    }
    unless filepath
      unless @silent
	STDERR.puts("WARNING: Skipping duplicate certificate #{org_filename}")
      end
    else
      (@cert_cache[name_hash] ||= []) << path(filepath)
    end
  end

  def link_hash_crl(org_filename, crl)
    name_hash = hash_name(crl.issuer)
    fingerprint = fingerprint(crl.to_der)
    filepath = link_hash(org_filename, name_hash, fingerprint) { |idx|
      "#{name_hash}.r#{idx}"
    }
    unless filepath
      unless @silent
	STDERR.puts("WARNING: Skipping duplicate CRL #{org_filename}")
      end
    else
      (@crl_cache[name_hash] ||= []) << path(filepath)
    end
  end

  def link_hash(org_filename, name, fingerprint)
    idx = 0
    filepath = nil
    while true
      filepath = yield(idx)
      break unless FileTest.symlink?(filepath) or FileTest.exist?(filepath)
      if @fingerprint_cache[filepath] == fingerprint
	return false
      end
      idx += 1
    end
    STDOUT.puts("#{org_filename} => #{filepath}") unless @silent
    symlink(org_filename, filepath)
    @fingerprint_cache[filepath] = fingerprint
    filepath
  end

  def symlink(from, to)
    begin
      File.symlink(from, to)
    rescue
      File.open(to, "w") do |f|
	f << File.read(from)
      end
    end
  end

  def path(filename)
    File.join(@dirpath, filename)
  end

  def hash_name(name)
    sprintf("%x", name.hash)
  end

  def fingerprint(der)
    Digest::MD5.hexdigest(der).upcase
  end
end

if $0 == __FILE__
  dirlist = ARGV
  dirlist << '/usr/ssl/certs' if dirlist.empty?
  dirlist.each do |dir|
    CHashDir.new(dir).hash_dir
  end
end
