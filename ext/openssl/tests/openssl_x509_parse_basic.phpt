--TEST--
openssl_x509_parse() basic test
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$cert = "file://" . dirname(__FILE__) . "/cert.crt";

var_dump(openssl_x509_parse($cert));
var_dump(openssl_x509_parse($cert, false));
?>
--EXPECTF--
array(12) {
  [u"name"]=>
  string(96) "/C=BR/ST=Rio Grande do Sul/L=Porto Alegre/CN=Henrique do N. Angelo/emailAddress=hnangelo@php.net"
  [u"subject"]=>
  array(5) {
    [u"C"]=>
    unicode(2) "BR"
    [u"ST"]=>
    unicode(17) "Rio Grande do Sul"
    [u"L"]=>
    unicode(12) "Porto Alegre"
    [u"CN"]=>
    unicode(21) "Henrique do N. Angelo"
    [u"emailAddress"]=>
    unicode(16) "hnangelo@php.net"
  }
  [u"hash"]=>
  string(8) "088c65c2"
  [u"issuer"]=>
  array(5) {
    [u"C"]=>
    unicode(2) "BR"
    [u"ST"]=>
    unicode(17) "Rio Grande do Sul"
    [u"L"]=>
    unicode(12) "Porto Alegre"
    [u"CN"]=>
    unicode(21) "Henrique do N. Angelo"
    [u"emailAddress"]=>
    unicode(16) "hnangelo@php.net"
  }
  [u"version"]=>
  int(2)
  [u"serialNumber"]=>
  string(20) "12593567369101004962"
  [u"validFrom"]=>
  string(13) "080630102843Z"
  [u"validTo"]=>
  string(13) "080730102843Z"
  [u"validFrom_time_t"]=>
  int(1214821723)
  [u"validTo_time_t"]=>
  int(1217413723)
  [u"purposes"]=>
  array(8) {
    [1]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(9) "sslclient"
    }
    [2]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(9) "sslserver"
    }
    [3]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(11) "nssslserver"
    }
    [4]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(9) "smimesign"
    }
    [5]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(12) "smimeencrypt"
    }
    [6]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(7) "crlsign"
    }
    [7]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(3) "any"
    }
    [8]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(10) "ocsphelper"
    }
  }
  [u"extensions"]=>
  array(3) {
    [u"subjectKeyIdentifier"]=>
    unicode(59) "DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D"
    [u"authorityKeyIdentifier"]=>
    unicode(202) "keyid:DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D
DirName:/C=BR/ST=Rio Grande do Sul/L=Porto Alegre/CN=Henrique do N. Angelo/emailAddress=hnangelo@php.net
serial:AE:C5:56:CC:72:37:50:A2
"
    [u"basicConstraints"]=>
    unicode(7) "CA:TRUE"
  }
}
array(12) {
  [u"name"]=>
  string(96) "/C=BR/ST=Rio Grande do Sul/L=Porto Alegre/CN=Henrique do N. Angelo/emailAddress=hnangelo@php.net"
  [u"subject"]=>
  array(5) {
    [u"countryName"]=>
    unicode(2) "BR"
    [u"stateOrProvinceName"]=>
    unicode(17) "Rio Grande do Sul"
    [u"localityName"]=>
    unicode(12) "Porto Alegre"
    [u"commonName"]=>
    unicode(21) "Henrique do N. Angelo"
    [u"emailAddress"]=>
    unicode(16) "hnangelo@php.net"
  }
  [u"hash"]=>
  string(8) "088c65c2"
  [u"issuer"]=>
  array(5) {
    [u"countryName"]=>
    unicode(2) "BR"
    [u"stateOrProvinceName"]=>
    unicode(17) "Rio Grande do Sul"
    [u"localityName"]=>
    unicode(12) "Porto Alegre"
    [u"commonName"]=>
    unicode(21) "Henrique do N. Angelo"
    [u"emailAddress"]=>
    unicode(16) "hnangelo@php.net"
  }
  [u"version"]=>
  int(2)
  [u"serialNumber"]=>
  string(20) "12593567369101004962"
  [u"validFrom"]=>
  string(13) "080630102843Z"
  [u"validTo"]=>
  string(13) "080730102843Z"
  [u"validFrom_time_t"]=>
  int(1214821723)
  [u"validTo_time_t"]=>
  int(1217413723)
  [u"purposes"]=>
  array(8) {
    [1]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(10) "SSL client"
    }
    [2]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(10) "SSL server"
    }
    [3]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(19) "Netscape SSL server"
    }
    [4]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(14) "S/MIME signing"
    }
    [5]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(17) "S/MIME encryption"
    }
    [6]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(11) "CRL signing"
    }
    [7]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(11) "Any Purpose"
    }
    [8]=>
    array(3) {
      [0]=>
      bool(true)
      [1]=>
      bool(true)
      [2]=>
      string(11) "OCSP helper"
    }
  }
  [u"extensions"]=>
  array(3) {
    [u"subjectKeyIdentifier"]=>
    unicode(59) "DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D"
    [u"authorityKeyIdentifier"]=>
    unicode(202) "keyid:DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D
DirName:/C=BR/ST=Rio Grande do Sul/L=Porto Alegre/CN=Henrique do N. Angelo/emailAddress=hnangelo@php.net
serial:AE:C5:56:CC:72:37:50:A2
"
    [u"basicConstraints"]=>
    unicode(7) "CA:TRUE"
  }
}
