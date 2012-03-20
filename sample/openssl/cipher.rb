#!/usr/bin/env ruby
require 'openssl'

def crypt_by_password(alg, pass, salt, text)
  puts "--Setup--"
  puts %(cipher alg:    "#{alg}")
  puts %(plain text:    "#{text}")
  puts %(password:      "#{pass}")
  puts %(salt:          "#{salt}")
  puts

  puts "--Encrypting--"
  enc = OpenSSL::Cipher::Cipher.new(alg)
  enc.encrypt
  enc.pkcs5_keyivgen(pass, salt)
  cipher =  enc.update(text)
  cipher << enc.final
  puts %(encrypted text: #{cipher.inspect})
  puts

  puts "--Decrypting--"
  dec = OpenSSL::Cipher::Cipher.new(alg)
  dec.decrypt
  dec.pkcs5_keyivgen(pass, salt)
  plain =  dec.update(cipher)
  plain << dec.final
  puts %(decrypted text: "#{plain}")
  puts
end

def ciphers
  ciphers = OpenSSL::Cipher.ciphers.sort
  ciphers.each{|i|
    if i.upcase != i && ciphers.include?(i.upcase)
      ciphers.delete(i)
    end
  }
  return ciphers
end

puts "Supported ciphers in #{OpenSSL::OPENSSL_VERSION}:"
ciphers.each_with_index{|name, i|
  printf("%-15s", name)
  puts if (i + 1) % 5 == 0
}
puts
puts

alg  = ARGV.shift || ciphers.first
pass = "secret password"
salt = "8 octets"        # or nil
text = "abcdefghijklmnopqrstuvwxyz"

crypt_by_password(alg, pass, salt, text)
