begin
  require 'http-access2'
rescue LoadError
  STDERR.puts("Cannot load http-access2.  CRL might not be fetched.")
end
require 'c_rehash'


class CrlStore
  def initialize(c_store)
    @c_store = c_store
    @c_store.hash_dir(true)
  end

  def find_crl(cert)
    do_find_crl(cert)
  end

private

  def do_find_crl(cert)
    unless ca = find_ca(cert)
      return nil
    end
    unless crlfiles = @c_store.get_crls(ca.subject)
      if crl = renew_crl(cert, ca)
	@c_store.add_crl(crl)
	return crl
      end
      return nil
    end
    crlfiles.each do |crlfile|
      next unless crl = load_crl(crlfile)
      if crl.next_update < Time.now
	if new_crl = renew_crl(cert, ca)
	  @c_store.delete_crl(crl)
	  @c_store.add_crl(new_crl)
	  crl = new_crl
	end
      end
      if check_valid(crl, ca)
	return crl
      end
    end
    nil
  end

  def find_ca(cert)
    @c_store.get_certs(cert.issuer).each do |cafile|
      ca = load_cert(cafile)
      if cert.verify(ca.public_key)
	return ca
      end
    end
    nil
  end

  def fetch(location)
    if /\AURI:(.*)\z/ =~ location
      begin
	c = HTTPAccess2::Client.new(ENV['http_proxy'] || ENV['HTTP_PROXY'])
	c.get_content($1)
      rescue NameError, StandardError
	nil
      end
    else
      nil
    end
  end

  def load_cert(certfile)
    load_cert_str(File.read(certfile))
  end

  def load_crl(crlfile)
    load_crl_str(File.read(crlfile))
  end

  def load_cert_str(cert_str)
    OpenSSL::X509::Certificate.new(cert_str)
  end

  def load_crl_str(crl_str)
    OpenSSL::X509::CRL.new(crl_str)
  end

  def check_valid(crl, ca)
    unless crl.verify(ca.public_key)
      return false
    end
    crl.last_update <= Time.now
  end

  RE_CDP = /\AcrlDistributionPoints\z/
  def get_cdp(cert)
    if cdp_ext = cert.extensions.find { |ext| RE_CDP =~ ext.oid }
      cdp_ext.value.chomp
    else
      false
    end
  end

  def renew_crl(cert, ca)
    if cdp = get_cdp(cert)
      if new_crl_str = fetch(cdp)
	new_crl = load_crl_str(new_crl_str)
	if check_valid(new_crl, ca)
	  return new_crl
	end
      end
    end
    false
  end
end

if $0 == __FILE__
  dir = "trust_certs"
  c_store = CHashDir.new(dir)
  s = CrlStore.new(c_store)
  c = OpenSSL::X509::Certificate.new(File.read("cert_store/google_codesign.pem"))
  p s.find_crl(c)
end
