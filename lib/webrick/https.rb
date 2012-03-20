#
# https.rb -- SSL/TLS enhancement for HTTPServer
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2001 GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: https.rb,v 1.15 2003/07/22 19:20:42 gotoyuzo Exp $

require 'webrick/ssl'

module WEBrick
  module Config
    HTTP.update(SSL)
  end

  class HTTPRequest
    attr_reader :cipher, :server_cert, :client_cert

    alias orig_parse parse

    def parse(socket=nil)
      if socket.respond_to?(:cert)
        @server_cert = socket.cert || @config[:SSLCertificate]
        @client_cert = socket.peer_cert
        @client_cert_chain = socket.peer_cert_chain
        @cipher      = socket.cipher
      end
      orig_parse(socket)
    end

    alias orig_parse_uri parse_uri

    def parse_uri(str, scheme="https")
      if server_cert
        return orig_parse_uri(str, scheme)
      end
      return orig_parse_uri(str)
    end
    private :parse_uri

    alias orig_meta_vars meta_vars

    def meta_vars
      meta = orig_meta_vars
      if server_cert
        meta["HTTPS"] = "on"
        meta["SSL_SERVER_CERT"] = @server_cert.to_pem
        meta["SSL_CLIENT_CERT"] = @client_cert ? @client_cert.to_pem : ""
        if @client_cert_chain
          @client_cert_chain.each_with_index{|cert, i|
            meta["SSL_CLIENT_CERT_CHAIN_#{i}"] = cert.to_pem
          }
        end
        meta["SSL_CIPHER"] = @cipher[0]
        meta["SSL_PROTOCOL"] = @cipher[1]
        meta["SSL_CIPHER_USEKEYSIZE"] = @cipher[2].to_s
        meta["SSL_CIPHER_ALGKEYSIZE"] = @cipher[3].to_s
      end
      meta
    end
  end
end
