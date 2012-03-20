require 'c_rehash'
require 'crlstore'


class CertStore
  include OpenSSL
  include X509

  attr_reader :self_signed_ca
  attr_reader :other_ca
  attr_reader :ee
  attr_reader :crl
  attr_reader :request

  def initialize(certs_dir)
    @certs_dir = certs_dir
    @c_store = CHashDir.new(@certs_dir)
    @c_store.hash_dir(true)
    @crl_store = CrlStore.new(@c_store)
    @x509store = Store.new
    @self_signed_ca = @other_ca = @ee = @crl = nil

    # Uncomment this line to let OpenSSL to check CRL for each certs.
    # @x509store.flags = V_FLAG_CRL_CHECK | V_FLAG_CRL_CHECK_ALL

    add_path
    scan_certs
  end

  def generate_cert(filename)
    @c_store.load_pem_file(filename)
  end

  def verify(cert)
    error, crl_map = do_verify(cert)
    if error
      [[false, cert, crl_map[cert.subject], error]]
    else
      @x509store.chain.collect { |c| [true, c, crl_map[c.subject], nil] }
    end
  end

  def match_cert(cert1, cert2)
    (cert1.issuer.cmp(cert2.issuer) == 0) and cert1.serial == cert2.serial
  end

  def is_ca?(cert)
    case guess_cert_type(cert)
    when CERT_TYPE_SELF_SIGNED
      true
    when CERT_TYPE_OTHER
      true
    else
      false
    end
  end

  def scan_certs
    @self_signed_ca = []
    @other_ca = []
    @ee = []
    @crl = []
    @request = []
    load_certs
  end

private

  def add_path
    @x509store.add_path(@certs_dir)
  end

  def do_verify(cert)
    error_map = {}
    crl_map = {}
    result = @x509store.verify(cert) do |ok, ctx|
      cert = ctx.current_cert
      if ctx.current_crl
	crl_map[cert.subject] = true
      end
      if ok
	if !ctx.current_crl
	  if crl = @crl_store.find_crl(cert)
	    crl_map[cert.subject] = true
	    if crl.revoked.find { |revoked| revoked.serial == cert.serial }
	      ok = false
	      error_string = 'certification revoked'
	    end
	  end
	end
      end
      error_map[cert.subject] = error_string if error_string
      ok
    end
    error = if result
	nil
      else
	error_map[cert.subject] || @x509store.error_string
      end
    return error, crl_map
  end

  def load_certs
    @c_store.get_certs.each do |certfile|
      cert = generate_cert(certfile)
      case guess_cert_type(cert)
      when CERT_TYPE_SELF_SIGNED
	@self_signed_ca << cert
      when CERT_TYPE_OTHER
	@other_ca << cert
      when CERT_TYPE_EE
	@ee << cert
      else
	raise "Unknown cert type."
      end
    end
    @c_store.get_crls.each do |crlfile|
      @crl << generate_cert(crlfile)
    end
  end

  CERT_TYPE_SELF_SIGNED = 0
  CERT_TYPE_OTHER = 1
  CERT_TYPE_EE = 2
  def guess_cert_type(cert)
    ca = self_signed = is_cert_self_signed(cert)
    cert.extensions.each do |ext|
      # Ignores criticality of extensions.  It's 'guess'ing.
      case ext.oid
      when 'basicConstraints'
	/CA:(TRUE|FALSE), pathlen:(\d+)/ =~ ext.value
	ca = ($1 == 'TRUE') unless ca
      when 'keyUsage'
	usage = ext.value.split(/\s*,\s*/)
	ca = usage.include?('Certificate Sign') unless ca
      when 'nsCertType'
	usage = ext.value.split(/\s*,\s*/)
	ca = usage.include?('SSL CA') unless ca
      end
    end
    if ca
      if self_signed
	CERT_TYPE_SELF_SIGNED
      else
	CERT_TYPE_OTHER
      end
    else
      CERT_TYPE_EE
    end
  end

  def is_cert_self_signed(cert)
    # cert.subject.cmp(cert.issuer) == 0
    cert.subject.to_s == cert.issuer.to_s
  end
end


if $0 == __FILE__
  c = CertStore.new("trust_certs")
end
