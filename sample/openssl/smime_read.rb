require 'optparse'
require 'openssl'
include OpenSSL

options = ARGV.getopts("c:k:C:")

cert_file = options["c"]
key_file  = options["k"]
ca_path   = options["C"]

data = $stdin.read

cert = X509::Certificate.new(File::read(cert_file))
key = PKey::RSA.new(File::read(key_file))
p7enc = PKCS7::read_smime(data)
data = p7enc.decrypt(key, cert)

store = X509::Store.new
store.add_path(ca_path)
p7sig = PKCS7::read_smime(data)
if p7sig.verify([], store)
  puts p7sig.data
end
