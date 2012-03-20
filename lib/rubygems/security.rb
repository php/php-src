#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems/exceptions'
require 'rubygems/gem_openssl'
require 'fileutils'

#
# = Signed Gems README
#
# == Table of Contents
# * Overview
# * Walkthrough
# * Command-Line Options
# * OpenSSL Reference
# * Bugs/TODO
# * About the Author
#
# == Overview
#
# Gem::Security implements cryptographic signatures in RubyGems.  The section
# below is a step-by-step guide to using signed gems and generating your own.
#
# == Walkthrough
#
# In order to start signing your gems, you'll need to build a private key and
# a self-signed certificate.  Here's how:
#
#   # build a private key and certificate for gemmaster@example.com
#   $ gem cert --build gemmaster@example.com
#
# This could take anywhere from 5 seconds to 10 minutes, depending on the
# speed of your computer (public key algorithms aren't exactly the speediest
# crypto algorithms in the world).  When it's finished, you'll see the files
# "gem-private_key.pem" and "gem-public_cert.pem" in the current directory.
#
# First things first: take the "gem-private_key.pem" file and move it
# somewhere private, preferably a directory only you have access to, a floppy
# (yuck!), a CD-ROM, or something comparably secure.  Keep your private key
# hidden; if it's compromised, someone can sign packages as you (note: PKI has
# ways of mitigating the risk of stolen keys; more on that later).
#
# Now, let's sign an existing gem.  I'll be using my Imlib2-Ruby bindings, but
# you can use whatever gem you'd like.  Open up your existing gemspec file and
# add the following lines:
#
#   # signing key and certificate chain
#   s.signing_key = '/mnt/floppy/gem-private_key.pem'
#   s.cert_chain  = ['gem-public_cert.pem']
#
# (Be sure to replace "/mnt/floppy" with the ultra-secret path to your private
# key).
#
# After that, go ahead and build your gem as usual.  Congratulations, you've
# just built your first signed gem!  If you peek inside your gem file, you'll
# see a couple of new files have been added:
#
#   $ tar tf tar tf Imlib2-Ruby-0.5.0.gem
#   data.tar.gz
#   data.tar.gz.sig
#   metadata.gz
#   metadata.gz.sig
#
# Now let's verify the signature.  Go ahead and install the gem, but add the
# following options: "-P HighSecurity", like this:
#
#   # install the gem with using the security policy "HighSecurity"
#   $ sudo gem install Imlib2-Ruby-0.5.0.gem -P HighSecurity
#
# The -P option sets your security policy -- we'll talk about that in just a
# minute.  Eh, what's this?
#
#   Attempting local installation of 'Imlib2-Ruby-0.5.0.gem'
#   ERROR:  Error installing gem Imlib2-Ruby-0.5.0.gem[.gem]: Couldn't
#   verify data signature: Untrusted Signing Chain Root: cert =
#   '/CN=gemmaster/DC=example/DC=com', error = 'path
#   "/root/.rubygems/trust/cert-15dbb43a6edf6a70a85d4e784e2e45312cff7030.pem"
#   does not exist'
#
# The culprit here is the security policy.  RubyGems has several different
# security policies.  Let's take a short break and go over the security
# policies.  Here's a list of the available security policies, and a brief
# description of each one:
#
# * NoSecurity - Well, no security at all.  Signed packages are treated like
#   unsigned packages.
# * LowSecurity - Pretty much no security.  If a package is signed then
#   RubyGems will make sure the signature matches the signing
#   certificate, and that the signing certificate hasn't expired, but
#   that's it.  A malicious user could easily circumvent this kind of
#   security.
# * MediumSecurity - Better than LowSecurity and NoSecurity, but still
#   fallible.  Package contents are verified against the signing
#   certificate, and the signing certificate is checked for validity,
#   and checked against the rest of the certificate chain (if you don't
#   know what a certificate chain is, stay tuned, we'll get to that).
#   The biggest improvement over LowSecurity is that MediumSecurity
#   won't install packages that are signed by untrusted sources.
#   Unfortunately, MediumSecurity still isn't totally secure -- a
#   malicious user can still unpack the gem, strip the signatures, and
#   distribute the gem unsigned.
# * HighSecurity - Here's the bugger that got us into this mess.
#   The HighSecurity policy is identical to the MediumSecurity policy,
#   except that it does not allow unsigned gems.  A malicious user
#   doesn't have a whole lot of options here; he can't modify the
#   package contents without invalidating the signature, and he can't
#   modify or remove signature or the signing certificate chain, or
#   RubyGems will simply refuse to install the package.  Oh well, maybe
#   he'll have better luck causing problems for CPAN users instead :).
#
# So, the reason RubyGems refused to install our shiny new signed gem was
# because it was from an untrusted source.  Well, my code is infallible
# (hah!), so I'm going to add myself as a trusted source.
#
# Here's how:
#
#     # add trusted certificate
#     gem cert --add gem-public_cert.pem
#
# I've added my public certificate as a trusted source.  Now I can install
# packages signed my private key without any hassle.  Let's try the install
# command above again:
#
#   # install the gem with using the HighSecurity policy (and this time
#   # without any shenanigans)
#   $ sudo gem install Imlib2-Ruby-0.5.0.gem -P HighSecurity
#
# This time RubyGems should accept your signed package and begin installing.
# While you're waiting for RubyGems to work it's magic, have a look at some of
# the other security commands:
#
#   Usage: gem cert [options]
#
#   Options:
#     -a, --add CERT          Add a trusted certificate.
#     -l, --list              List trusted certificates.
#     -r, --remove STRING     Remove trusted certificates containing STRING.
#     -b, --build EMAIL_ADDR  Build private key and self-signed certificate
#                             for EMAIL_ADDR.
#     -C, --certificate CERT  Certificate for --sign command.
#     -K, --private-key KEY   Private key for --sign command.
#     -s, --sign NEWCERT      Sign a certificate with my key and certificate.
#
# (By the way, you can pull up this list any time you'd like by typing "gem
# cert --help")
#
# Hmm.  We've already covered the "--build" option, and the "--add", "--list",
# and "--remove" commands seem fairly straightforward; they allow you to add,
# list, and remove the certificates in your trusted certificate list.  But
# what's with this "--sign" option?
#
# To answer that question, let's take a look at "certificate chains", a
# concept I mentioned earlier.  There are a couple of problems with
# self-signed certificates: first of all, self-signed certificates don't offer
# a whole lot of security.  Sure, the certificate says Yukihiro Matsumoto, but
# how do I know it was actually generated and signed by matz himself unless he
# gave me the certificate in person?
#
# The second problem is scalability.  Sure, if there are 50 gem authors, then
# I have 50 trusted certificates, no problem.  What if there are 500 gem
# authors?  1000?  Having to constantly add new trusted certificates is a
# pain, and it actually makes the trust system less secure by encouraging
# RubyGems users to blindly trust new certificates.
#
# Here's where certificate chains come in.  A certificate chain establishes an
# arbitrarily long chain of trust between an issuing certificate and a child
# certificate.  So instead of trusting certificates on a per-developer basis,
# we use the PKI concept of certificate chains to build a logical hierarchy of
# trust.  Here's a hypothetical example of a trust hierarchy based (roughly)
# on geography:
#
#
#                         --------------------------
#                         | rubygems@rubyforge.org |
#                         --------------------------
#                                     |
#                   -----------------------------------
#                   |                                 |
#       ----------------------------    -----------------------------
#       | seattle.rb@zenspider.com |    | dcrubyists@richkilmer.com |
#       ----------------------------    -----------------------------
#            |                |                 |             |
#     ---------------   ----------------   -----------   --------------
#     | alf@seattle |   | bob@portland |   | pabs@dc |   | tomcope@dc |
#     ---------------   ----------------   -----------   --------------
#
#
# Now, rather than having 4 trusted certificates (one for alf@seattle,
# bob@portland, pabs@dc, and tomecope@dc), a user could actually get by with 1
# certificate: the "rubygems@rubyforge.org" certificate.  Here's how it works:
#
# I install "Alf2000-Ruby-0.1.0.gem", a package signed by "alf@seattle".  I've
# never heard of "alf@seattle", but his certificate has a valid signature from
# the "seattle.rb@zenspider.com" certificate, which in turn has a valid
# signature from the "rubygems@rubyforge.org" certificate.  Voila!  At this
# point, it's much more reasonable for me to trust a package signed by
# "alf@seattle", because I can establish a chain to "rubygems@rubyforge.org",
# which I do trust.
#
# And the "--sign" option allows all this to happen.  A developer creates
# their build certificate with the "--build" option, then has their
# certificate signed by taking it with them to their next regional Ruby meetup
# (in our hypothetical example), and it's signed there by the person holding
# the regional RubyGems signing certificate, which is signed at the next
# RubyConf by the holder of the top-level RubyGems certificate.  At each point
# the issuer runs the same command:
#
#   # sign a certificate with the specified key and certificate
#   # (note that this modifies client_cert.pem!)
#   $ gem cert -K /mnt/floppy/issuer-priv_key.pem -C issuer-pub_cert.pem
#      --sign client_cert.pem
#
# Then the holder of issued certificate (in this case, our buddy
# "alf@seattle"), can start using this signed certificate to sign RubyGems.
# By the way, in order to let everyone else know about his new fancy signed
# certificate, "alf@seattle" would change his gemspec file to look like this:
#
#   # signing key (still kept in an undisclosed location!)
#   s.signing_key = '/mnt/floppy/alf-private_key.pem'
#
#   # certificate chain (includes the issuer certificate now too)
#   s.cert_chain  = ['/home/alf/doc/seattlerb-public_cert.pem',
#                    '/home/alf/doc/alf_at_seattle-public_cert.pem']
#
# Obviously, this RubyGems trust infrastructure doesn't exist yet.  Also, in
# the "real world" issuers actually generate the child certificate from a
# certificate request, rather than sign an existing certificate.  And our
# hypothetical infrastructure is missing a certificate revocation system.
# These are that can be fixed in the future...
#
# I'm sure your new signed gem has finished installing by now (unless you're
# installing rails and all it's dependencies, that is ;D).  At this point you
# should know how to do all of these new and interesting things:
#
# * build a gem signing key and certificate
# * modify your existing gems to support signing
# * adjust your security policy
# * modify your trusted certificate list
# * sign a certificate
#
# If you've got any questions, feel free to contact me at the email address
# below.  The next couple of sections
#
#
# == Command-Line Options
#
# Here's a brief summary of the certificate-related command line options:
#
#   gem install
#     -P, --trust-policy POLICY        Specify gem trust policy.
#
#   gem cert
#     -a, --add CERT                   Add a trusted certificate.
#     -l, --list                       List trusted certificates.
#     -r, --remove STRING              Remove trusted certificates containing
#                                      STRING.
#     -b, --build EMAIL_ADDR           Build private key and self-signed
#                                      certificate for EMAIL_ADDR.
#     -C, --certificate CERT           Certificate for --sign command.
#     -K, --private-key KEY            Private key for --sign command.
#     -s, --sign NEWCERT               Sign a certificate with my key and
#                                      certificate.
#
# A more detailed description of each options is available in the walkthrough
# above.
#
# == Manually verifying signatures
#
# In case you don't trust RubyGems you can verify gem signatures manually:
#
# 1. Fetch and unpack the gem
#
#     gem fetch some_signed_gem
#     tar -xf some_signed_gem-1.0.gem
#
# 2. Grab the public key from the gemspec
#
#     gem spec some_signed_gem-1.0.gem cert_chain | \
#       ruby -pe 'sub(/^ +/, "")' > public_key.crt
#
# 3. Generate a SHA1 hash of the data.tar.gz
#
#     openssl dgst -sha1 < data.tar.gz > my.hash
#
# 4. Verify the signature
#
#     openssl rsautl -verify -inkey public_key.crt -certin \
#       -in data.tar.gz.sig > verified.hash
#
# 5. Compare your hash to the verified hash
#
#     diff -s verified.hash my.hash
#
# 6. Repeat 5 and 6 with metadata.gz
#
# == OpenSSL Reference
#
# The .pem files generated by --build and --sign are just basic OpenSSL PEM
# files.  Here's a couple of useful commands for manipulating them:
#
#   # convert a PEM format X509 certificate into DER format:
#   # (note: Windows .cer files are X509 certificates in DER format)
#   $ openssl x509 -in input.pem -outform der -out output.der
#
#   # print out the certificate in a human-readable format:
#   $ openssl x509 -in input.pem -noout -text
#
# And you can do the same thing with the private key file as well:
#
#   # convert a PEM format RSA key into DER format:
#   $ openssl rsa -in input_key.pem -outform der -out output_key.der
#
#   # print out the key in a human readable format:
#   $ openssl rsa -in input_key.pem -noout -text
#
# == Bugs/TODO
#
# * There's no way to define a system-wide trust list.
# * custom security policies (from a YAML file, etc)
# * Simple method to generate a signed certificate request
# * Support for OCSP, SCVP, CRLs, or some other form of cert
#   status check (list is in order of preference)
# * Support for encrypted private keys
# * Some sort of semi-formal trust hierarchy (see long-winded explanation
#   above)
# * Path discovery (for gem certificate chains that don't have a self-signed
#   root) -- by the way, since we don't have this, THE ROOT OF THE CERTIFICATE
#   CHAIN MUST BE SELF SIGNED if Policy#verify_root is true (and it is for the
#   MediumSecurity and HighSecurity policies)
# * Better explanation of X509 naming (ie, we don't have to use email
#   addresses)
# * Possible alternate signing mechanisms (eg, via PGP).  this could be done
#   pretty easily by adding a :signing_type attribute to the gemspec, then add
#   the necessary support in other places
# * Honor AIA field (see note about OCSP above)
# * Maybe honor restriction extensions?
# * Might be better to store the certificate chain as a PKCS#7 or PKCS#12
#   file, instead of an array embedded in the metadata.  ideas?
# * Possibly embed signature and key algorithms into metadata (right now
#   they're assumed to be the same as what's set in Gem::Security::OPT)
#
# == About the Author
#
# Paul Duncan <pabs@pablotron.org>
# http://pablotron.org/

module Gem::Security

  ##
  # Gem::Security default exception type

  class Exception < Gem::Exception; end

  ##
  # Default options for most of the methods below

  OPT = {
    # private key options
    :key_algo   => Gem::SSL::PKEY_RSA,
    :key_size   => 2048,

    # public cert options
    :cert_age   => 365 * 24 * 3600, # 1 year
    :dgst_algo  => Gem::SSL::DIGEST_SHA1,

    # x509 certificate extensions
    :cert_exts  => {
      'basicConstraints'      => 'CA:FALSE',
      'subjectKeyIdentifier'  => 'hash',
      'keyUsage'              => 'keyEncipherment,dataEncipherment,digitalSignature',
    },

    # save the key and cert to a file in build_self_signed_cert()?
    :save_key   => true,
    :save_cert  => true,

    # if you define either of these, then they'll be used instead of
    # the output_fmt macro below
    :save_key_path => nil,
    :save_cert_path => nil,

    # output name format for self-signed certs
    :output_fmt => 'gem-%s.pem',
    :munge_re   => Regexp.new(/[^a-z0-9_.-]+/),

    # output directory for trusted certificate checksums
    :trust_dir => File.join(Gem.user_home, '.gem', 'trust'),

    # default permissions for trust directory and certs
    :perms => {
      :trust_dir      => 0700,
      :trusted_cert   => 0600,
      :signing_cert   => 0600,
      :signing_key    => 0600,
    },
  }

  ##
  # A Gem::Security::Policy object encapsulates the settings for verifying
  # signed gem files.  This is the base class.  You can either declare an
  # instance of this or use one of the preset security policies below.

  class Policy
    attr_accessor :verify_data, :verify_signer, :verify_chain,
      :verify_root, :only_trusted, :only_signed

    #
    # Create a new Gem::Security::Policy object with the given mode and
    # options.
    #
    def initialize(policy = {}, opt = {})
      # set options
      @opt = Gem::Security::OPT.merge(opt)

      # build policy
      policy.each_pair do |key, val|
        case key
        when :verify_data   then @verify_data   = val
        when :verify_signer then @verify_signer = val
        when :verify_chain  then @verify_chain  = val
        when :verify_root   then @verify_root   = val
        when :only_trusted  then @only_trusted  = val
        when :only_signed   then @only_signed   = val
        end
      end
    end

    #
    # Get the path to the file for this cert.
    #
    def self.trusted_cert_path(cert, opt = {})
      opt = Gem::Security::OPT.merge(opt)

      # get digest algorithm, calculate checksum of root.subject
      algo = opt[:dgst_algo]
      dgst = algo.hexdigest(cert.subject.to_s)

      # build path to trusted cert file
      name = "cert-#{dgst}.pem"

      # join and return path components
      File::join(opt[:trust_dir], name)
    end

    #
    # Verify that the gem data with the given signature and signing chain
    # matched this security policy at the specified time.
    #
    def verify_gem(signature, data, chain, time = Time.now)
      Gem.ensure_ssl_available
      cert_class = OpenSSL::X509::Certificate
      exc = Gem::Security::Exception
      chain ||= []

      chain = chain.map{ |str| cert_class.new(str) }
      signer, ch_len = chain[-1], chain.size

      # make sure signature is valid
      if @verify_data
        # get digest algorithm (TODO: this should be configurable)
        dgst = @opt[:dgst_algo]

        # verify the data signature (this is the most important part, so don't
        # screw it up :D)
        v = signer.public_key.verify(dgst.new, signature, data)
        raise exc, "Invalid Gem Signature" unless v

        # make sure the signer is valid
        if @verify_signer
          # make sure the signing cert is valid right now
          v = signer.check_validity(nil, time)
          raise exc, "Invalid Signature: #{v[:desc]}" unless v[:is_valid]
        end
      end

      # make sure the certificate chain is valid
      if @verify_chain
        # iterate down over the chain and verify each certificate against it's
        # issuer
        (ch_len - 1).downto(1) do |i|
          issuer, cert = chain[i - 1, 2]
          v = cert.check_validity(issuer, time)
          raise exc, "%s: cert = '%s', error = '%s'" % [
              'Invalid Signing Chain', cert.subject, v[:desc]
          ] unless v[:is_valid]
        end

        # verify root of chain
        if @verify_root
          # make sure root is self-signed
          root = chain[0]
          raise exc, "%s: %s (subject = '%s', issuer = '%s')" % [
              'Invalid Signing Chain Root',
              'Subject does not match Issuer for Gem Signing Chain',
              root.subject.to_s,
              root.issuer.to_s,
          ] unless root.issuer.to_s == root.subject.to_s

          # make sure root is valid
          v = root.check_validity(root, time)
          raise exc, "%s: cert = '%s', error = '%s'" % [
              'Invalid Signing Chain Root', root.subject, v[:desc]
          ] unless v[:is_valid]

          # verify that the chain root is trusted
          if @only_trusted
            # get digest algorithm, calculate checksum of root.subject
            algo = @opt[:dgst_algo]
            path = Gem::Security::Policy.trusted_cert_path(root, @opt)

            # check to make sure trusted path exists
            raise exc, "%s: cert = '%s', error = '%s'" % [
                'Untrusted Signing Chain Root',
                root.subject.to_s,
                "path \"#{path}\" does not exist",
            ] unless File.exist?(path)

            # load calculate digest from saved cert file
            save_cert = OpenSSL::X509::Certificate.new(File.read(path))
            save_dgst = algo.digest(save_cert.public_key.to_s)

            # create digest of public key
            pkey_str = root.public_key.to_s
            cert_dgst = algo.digest(pkey_str)

            # now compare the two digests, raise exception
            # if they don't match
            raise exc, "%s: %s (saved = '%s', root = '%s')" % [
                'Invalid Signing Chain Root',
                "Saved checksum doesn't match root checksum",
                save_dgst, cert_dgst,
            ] unless save_dgst == cert_dgst
          end
        end

        # return the signing chain
        chain.map { |cert| cert.subject }
      end
    end
  end

  ##
  # No security policy: all package signature checks are disabled.

  NoSecurity = Policy.new(
    :verify_data      => false,
    :verify_signer    => false,
    :verify_chain     => false,
    :verify_root      => false,
    :only_trusted     => false,
    :only_signed      => false
  )

  ##
  # AlmostNo security policy: only verify that the signing certificate is the
  # one that actually signed the data.  Make no attempt to verify the signing
  # certificate chain.
  #
  # This policy is basically useless. better than nothing, but can still be
  # easily spoofed, and is not recommended.

  AlmostNoSecurity = Policy.new(
    :verify_data      => true,
    :verify_signer    => false,
    :verify_chain     => false,
    :verify_root      => false,
    :only_trusted     => false,
    :only_signed      => false
  )

  ##
  # Low security policy: only verify that the signing certificate is actually
  # the gem signer, and that the signing certificate is valid.
  #
  # This policy is better than nothing, but can still be easily spoofed, and
  # is not recommended.

  LowSecurity = Policy.new(
    :verify_data      => true,
    :verify_signer    => true,
    :verify_chain     => false,
    :verify_root      => false,
    :only_trusted     => false,
    :only_signed      => false
  )

  ##
  # Medium security policy: verify the signing certificate, verify the signing
  # certificate chain all the way to the root certificate, and only trust root
  # certificates that we have explicitly allowed trust for.
  #
  # This security policy is reasonable, but it allows unsigned packages, so a
  # malicious person could simply delete the package signature and pass the
  # gem off as unsigned.

  MediumSecurity = Policy.new(
    :verify_data      => true,
    :verify_signer    => true,
    :verify_chain     => true,
    :verify_root      => true,
    :only_trusted     => true,
    :only_signed      => false
  )

  ##
  # High security policy: only allow signed gems to be installed, verify the
  # signing certificate, verify the signing certificate chain all the way to
  # the root certificate, and only trust root certificates that we have
  # explicitly allowed trust for.
  #
  # This security policy is significantly more difficult to bypass, and offers
  # a reasonable guarantee that the contents of the gem have not been altered.

  HighSecurity = Policy.new(
    :verify_data      => true,
    :verify_signer    => true,
    :verify_chain     => true,
    :verify_root      => true,
    :only_trusted     => true,
    :only_signed      => true
  )

  ##
  # Hash of configured security policies

  Policies = {
    'NoSecurity'       => NoSecurity,
    'AlmostNoSecurity' => AlmostNoSecurity,
    'LowSecurity'      => LowSecurity,
    'MediumSecurity'   => MediumSecurity,
    'HighSecurity'     => HighSecurity,
  }

  ##
  # Sign the cert cert with @signing_key and @signing_cert, using the digest
  # algorithm opt[:dgst_algo]. Returns the newly signed certificate.

  def self.sign_cert(cert, signing_key, signing_cert, opt = {})
    opt = OPT.merge(opt)

    cert.issuer = signing_cert.subject
    cert.sign signing_key, opt[:dgst_algo].new

    cert
  end

  ##
  # Make sure the trust directory exists.  If it does exist, make sure it's
  # actually a directory.  If not, then create it with the appropriate
  # permissions.

  def self.verify_trust_dir(path, perms)
    # if the directory exists, then make sure it is in fact a directory.  if
    # it doesn't exist, then create it with the appropriate permissions
    if File.exist?(path)
      # verify that the trust directory is actually a directory
      unless File.directory?(path)
        err = "trust directory #{path} isn't a directory"
        raise Gem::Security::Exception, err
      end
    else
      # trust directory doesn't exist, so create it with permissions
      FileUtils.mkdir_p(path)
      FileUtils.chmod(perms, path)
    end
  end

  ##
  # Build a certificate from the given DN and private key.

  def self.build_cert(name, key, opt = {})
    Gem.ensure_ssl_available
    opt = OPT.merge opt

    cert = OpenSSL::X509::Certificate.new

    cert.not_after  = Time.now + opt[:cert_age]
    cert.not_before = Time.now
    cert.public_key = key.public_key
    cert.serial     = 0
    cert.subject    = name
    cert.version    = 2

    ef = OpenSSL::X509::ExtensionFactory.new nil, cert

    cert.extensions = opt[:cert_exts].map do |ext_name, value|
      ef.create_extension ext_name, value
    end

    i_key  = opt[:issuer_key]  || key
    i_cert = opt[:issuer_cert] || cert

    cert = sign_cert cert, i_key, i_cert, opt

    cert
  end

  ##
  # Build a self-signed certificate for the given email address.

  def self.build_self_signed_cert(email_addr, opt = {})
    Gem.ensure_ssl_available
    opt = OPT.merge(opt)
    path = { :key => nil, :cert => nil }

    name = email_to_name email_addr, opt[:munge_re]

    key = opt[:key_algo].new opt[:key_size]

    verify_trust_dir opt[:trust_dir], opt[:perms][:trust_dir]

    if opt[:save_key] then
      path[:key] = opt[:save_key_path] || (opt[:output_fmt] % 'private_key')

      open path[:key], 'wb' do |io|
        io.chmod opt[:perms][:signing_key]
        io.write key.to_pem
      end
    end

    cert = build_cert name, key, opt

    if opt[:save_cert] then
      path[:cert] = opt[:save_cert_path] || (opt[:output_fmt] % 'public_cert')

      open path[:cert], 'wb' do |file|
        file.chmod opt[:perms][:signing_cert]
        file.write cert.to_pem
      end
    end

    { :key => key, :cert => cert,
      :key_path => path[:key], :cert_path => path[:cert] }
  end

  ##
  # Turns +email_address+ into an OpenSSL::X509::Name

  def self.email_to_name email_address, munge_re
    cn, dcs = email_address.split '@'

    dcs = dcs.split '.'

    cn = cn.gsub munge_re, '_'

    dcs = dcs.map do |dc|
      dc.gsub munge_re, '_'
    end

    name = "CN=#{cn}/" << dcs.map { |dc| "DC=#{dc}" }.join('/')

    OpenSSL::X509::Name.parse name
  end

  ##
  # Add certificate to trusted cert list.
  #
  # Note: At the moment these are stored in OPT[:trust_dir], although that
  # directory may change in the future.

  def self.add_trusted_cert(cert, opt = {})
    opt = OPT.merge(opt)

    # get destination path
    path = Gem::Security::Policy.trusted_cert_path(cert, opt)

    # verify trust directory (can't write to nowhere, you know)
    verify_trust_dir(opt[:trust_dir], opt[:perms][:trust_dir])

    # write cert to output file
    File.open(path, 'wb') do |file|
      file.chmod(opt[:perms][:trusted_cert])
      file.write(cert.to_pem)
    end

    # return nil
    nil
  end

  ##
  # Basic OpenSSL-based package signing class.

  class Signer

    attr_accessor :cert_chain
    attr_accessor :key

    def initialize(key, cert_chain)
      Gem.ensure_ssl_available
      @algo = Gem::Security::OPT[:dgst_algo]
      @key, @cert_chain = key, cert_chain

      # check key, if it's a file, and if it's key, leave it alone
      if @key && !@key.kind_of?(OpenSSL::PKey::PKey)
        @key = OpenSSL::PKey::RSA.new(File.read(@key))
      end

      # check cert chain, if it's a file, load it, if it's cert data, convert
      # it into a cert object, and if it's a cert object, leave it alone
      if @cert_chain
        @cert_chain = @cert_chain.map do |cert|
          # check cert, if it's a file, load it, if it's cert data, convert it
          # into a cert object, and if it's a cert object, leave it alone
          if cert && !cert.kind_of?(OpenSSL::X509::Certificate)
            cert = File.read(cert) if File::exist?(cert)
            cert = OpenSSL::X509::Certificate.new(cert)
          end
          cert
        end
      end
    end

    ##
    # Sign data with given digest algorithm

    def sign(data)
      @key.sign(@algo.new, data)
    end

  end

end

