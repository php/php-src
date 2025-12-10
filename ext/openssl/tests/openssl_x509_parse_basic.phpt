--TEST--
openssl_x509_parse() tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER >= 0x30200000) die('skip For OpenSSL < 3.2');
?>
--FILE--
<?php
$cert = "file://" . __DIR__ . "/crit.crt";

$parsedCert = openssl_x509_parse($cert);
var_dump($parsedCert === openssl_x509_parse(openssl_x509_read($cert)));
var_dump($parsedCert);
var_dump(openssl_x509_parse($cert, false));
?>
--EXPECTF--
bool(true)
array(16) {
  ["name"]=>
  string(96) "/CN=Henrique do N. Angelo/C=BR/ST=Rio Grande do Sul/L=Porto Alegre/emailAddress=hnangelo@php.net"
  ["subject"]=>
  array(5) {
    ["CN"]=>
    string(21) "Henrique do N. Angelo"
    ["C"]=>
    string(2) "BR"
    ["ST"]=>
    string(17) "Rio Grande do Sul"
    ["L"]=>
    string(12) "Porto Alegre"
    ["emailAddress"]=>
    string(16) "hnangelo@php.net"
  }
  ["hash"]=>
  string(8) "%s"
  ["issuer"]=>
  array(5) {
    ["CN"]=>
    string(21) "Henrique do N. Angelo"
    ["C"]=>
    string(2) "BR"
    ["ST"]=>
    string(17) "Rio Grande do Sul"
    ["L"]=>
    string(12) "Porto Alegre"
    ["emailAddress"]=>
    string(16) "hnangelo@php.net"
  }
  ["version"]=>
  int(2)
  ["serialNumber"]=>
  string(42) "0x5EE94A5F3A71AF7DEC57FD8BC08D3ECA1A540198"
  ["serialNumberHex"]=>
  string(40) "5EE94A5F3A71AF7DEC57FD8BC08D3ECA1A540198"
  ["validFrom"]=>
  string(13) "251002180630Z"
  ["validTo"]=>
  string(13) "261002180630Z"
  ["validFrom_time_t"]=>
  int(1759428390)
  ["validTo_time_t"]=>
  int(1790964390)
  ["signatureTypeSN"]=>
  string(8) "RSA-SHA1"
  ["signatureTypeLN"]=>
  string(21) "sha1WithRSAEncryption"
  ["signatureTypeNID"]=>
  int(65)
  ["purposes"]=>
  array(9) {
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
    [9]=>
    array(3) {
      [0]=>
      bool(false)
      [1]=>
      bool(true)
      [2]=>
      string(13) "timestampsign"
    }
  }
  ["extensions"]=>
  array(4) {
    ["subjectKeyIdentifier"]=>
    string(59) "DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D"
    ["authorityKeyIdentifier"]=>
    string(%d) "DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D"
    ["basicConstraints"]=>
    string(7) "CA:TRUE"
  }
  ["criticalExtensions"]=>
  array(1) {
    [0]=>
    string(16) "basicConstraints"
  }
}
array(16) {
  ["name"]=>
  string(96) "/CN=Henrique do N. Angelo/C=BR/ST=Rio Grande do Sul/L=Porto Alegre/emailAddress=hnangelo@php.net"
  ["subject"]=>
  array(5) {
    ["commonName"]=>
    string(21) "Henrique do N. Angelo"
    ["countryName"]=>
    string(2) "BR"
    ["stateOrProvinceName"]=>
    string(17) "Rio Grande do Sul"
    ["localityName"]=>
    string(12) "Porto Alegre"
    ["emailAddress"]=>
    string(16) "hnangelo@php.net"
  }
  ["hash"]=>
  string(8) "%s"
  ["issuer"]=>
  array(5) {
    ["commonName"]=>
    string(21) "Henrique do N. Angelo"
    ["countryName"]=>
    string(2) "BR"
    ["stateOrProvinceName"]=>
    string(17) "Rio Grande do Sul"
    ["localityName"]=>
    string(12) "Porto Alegre"
    ["emailAddress"]=>
    string(16) "hnangelo@php.net"
  }
  ["version"]=>
  int(2)
  ["serialNumber"]=>
  string(42) "0x5EE94A5F3A71AF7DEC57FD8BC08D3ECA1A540198"
  ["serialNumberHex"]=>
  string(40) "5EE94A5F3A71AF7DEC57FD8BC08D3ECA1A540198"
  ["validFrom"]=>
  string(13) "251002180630Z"
  ["validTo"]=>
  string(13) "261002180630Z"
  ["validFrom_time_t"]=>
  int(1759428390)
  ["validTo_time_t"]=>
  int(1790964390)
  ["signatureTypeSN"]=>
  string(8) "RSA-SHA1"
  ["signatureTypeLN"]=>
  string(21) "sha1WithRSAEncryption"
  ["signatureTypeNID"]=>
  int(65)
  ["purposes"]=>
  array(9) {
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
    [9]=>
    array(3) {
      [0]=>
      bool(false)
      [1]=>
      bool(true)
      [2]=>
      string(18) "Time Stamp signing"
    }
  }
  ["extensions"]=>
  array(4) {
    ["subjectKeyIdentifier"]=>
    string(59) "DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D"
    ["authorityKeyIdentifier"]=>
    string(%d) "DB:7E:40:72:BD:5C:35:85:EC:29:29:81:12:E8:62:68:6A:B7:3F:7D"
    ["basicConstraints:critical"]=>
    bool(true)
    ["basicConstraints"]=>
    string(7) "CA:TRUE"
  }
}
