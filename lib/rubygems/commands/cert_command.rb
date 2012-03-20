require 'rubygems/command'
require 'rubygems/security'

class Gem::Commands::CertCommand < Gem::Command

  def initialize
    super 'cert', 'Manage RubyGems certificates and signing settings'

    add_option('-a', '--add CERT',
               'Add a trusted certificate.') do |value, options|
      cert = OpenSSL::X509::Certificate.new(File.read(value))
      Gem::Security.add_trusted_cert(cert)
      say "Added '#{cert.subject.to_s}'"
    end

    add_option('-l', '--list',
               'List trusted certificates.') do |value, options|
      glob_str = File::join(Gem::Security::OPT[:trust_dir], '*.pem')
      Dir::glob(glob_str) do |path|
        begin
          cert = OpenSSL::X509::Certificate.new(File.read(path))
          # this could probably be formatted more gracefully
          say cert.subject.to_s
        rescue OpenSSL::X509::CertificateError
          next
        end
      end
    end

    add_option('-r', '--remove STRING',
               'Remove trusted certificates containing',
               'STRING.') do |value, options|
      trust_dir = Gem::Security::OPT[:trust_dir]
      glob_str = File::join(trust_dir, '*.pem')

      Dir::glob(glob_str) do |path|
        begin
          cert = OpenSSL::X509::Certificate.new(File.read(path))
          if cert.subject.to_s.downcase.index(value)
            say "Removed '#{cert.subject.to_s}'"
            File.unlink(path)
          end
        rescue OpenSSL::X509::CertificateError
          next
        end
      end
    end

    add_option('-b', '--build EMAIL_ADDR',
               'Build private key and self-signed',
               'certificate for EMAIL_ADDR.') do |value, options|
      vals = Gem::Security.build_self_signed_cert(value)
      FileUtils.chmod 0600, vals[:key_path]
      say "Public Cert: #{vals[:cert_path]}"
      say "Private Key: #{vals[:key_path]}"
      say "Don't forget to move the key file to somewhere private..."
    end

    add_option('-C', '--certificate CERT',
               'Certificate for --sign command.') do |value, options|
      cert = OpenSSL::X509::Certificate.new(File.read(value))
      options[:issuer_cert] = cert
    end

    add_option('-K', '--private-key KEY',
               'Private key for --sign command.') do |value, options|
      key = OpenSSL::PKey::RSA.new(File.read(value))
      options[:issuer_key] = key
    end

    add_option('-s', '--sign NEWCERT',
               'Sign a certificate with my key and',
               'certificate.') do |value, options|
      cert = OpenSSL::X509::Certificate.new(File.read(value))
      my_cert = options[:issuer_cert]
      my_key = options[:issuer_key]
      cert = Gem::Security.sign_cert(cert, my_key, my_cert)
      File.open(value, 'wb') { |file| file.write(cert.to_pem) }
    end
  end

  def execute
  end

end

