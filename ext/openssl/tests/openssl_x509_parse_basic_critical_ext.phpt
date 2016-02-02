--TEST--
openssl_x509_parse() basic test with critical extension
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; 
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$cert = "file://" . dirname(__FILE__) . "/cert_digicert.crt";

var_dump(openssl_x509_parse($cert));
var_dump(openssl_x509_parse($cert, false));
?>
--EXPECTF--
array(16) {
  ["name"]=>
  string(71) "/C=US/O=DigiCert Inc/OU=www.digicert.com/CN=DigiCert Assured ID Root CA"
  ["subject"]=>
  array(4) {
    ["C"]=>
    string(2) "US"
    ["O"]=>
    string(12) "DigiCert Inc"
    ["OU"]=>
    string(16) "www.digicert.com"
    ["CN"]=>
    string(27) "DigiCert Assured ID Root CA"
  }
  ["hash"]=>
  string(8) "b1159c4c"
  ["issuer"]=>
  array(4) {
    ["C"]=>
    string(2) "US"
    ["O"]=>
    string(12) "DigiCert Inc"
    ["OU"]=>
    string(16) "www.digicert.com"
    ["CN"]=>
    string(27) "DigiCert Assured ID Root CA"
  }
  ["version"]=>
  int(2)
  ["serialNumber"]=>
  string(38) "17154717934120587862167794914071425081"
  ["validFrom"]=>
  string(13) "061110000000Z"
  ["validTo"]=>
  string(13) "311110000000Z"
  ["validFrom_time_t"]=>
  int(1163116800)
  ["validTo_time_t"]=>
  int(1952035200)
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
      bool(false)
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
      bool(false)
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
    ["keyUsage"]=>
    string(45) "Digital Signature, Certificate Sign, CRL Sign"
    ["basicConstraints"]=>
    string(7) "CA:TRUE"
    ["subjectKeyIdentifier"]=>
    string(59) "45:EB:A2:AF:F4:92:CB:82:31:2D:51:8B:A7:A7:21:9D:F3:6D:C8:0F"
    ["authorityKeyIdentifier"]=>
    string(66) "keyid:45:EB:A2:AF:F4:92:CB:82:31:2D:51:8B:A7:A7:21:9D:F3:6D:C8:0F
"
  }
  ["criticalExtensions"]=>
  array(2) {
    [0]=>
    string(8) "keyUsage"
    [1]=>
    string(16) "basicConstraints"
  }
}
array(16) {
  ["name"]=>
  string(71) "/C=US/O=DigiCert Inc/OU=www.digicert.com/CN=DigiCert Assured ID Root CA"
  ["subject"]=>
  array(4) {
    ["countryName"]=>
    string(2) "US"
    ["organizationName"]=>
    string(12) "DigiCert Inc"
    ["organizationalUnitName"]=>
    string(16) "www.digicert.com"
    ["commonName"]=>
    string(27) "DigiCert Assured ID Root CA"
  }
  ["hash"]=>
  string(8) "b1159c4c"
  ["issuer"]=>
  array(4) {
    ["countryName"]=>
    string(2) "US"
    ["organizationName"]=>
    string(12) "DigiCert Inc"
    ["organizationalUnitName"]=>
    string(16) "www.digicert.com"
    ["commonName"]=>
    string(27) "DigiCert Assured ID Root CA"
  }
  ["version"]=>
  int(2)
  ["serialNumber"]=>
  string(38) "17154717934120587862167794914071425081"
  ["validFrom"]=>
  string(13) "061110000000Z"
  ["validTo"]=>
  string(13) "311110000000Z"
  ["validFrom_time_t"]=>
  int(1163116800)
  ["validTo_time_t"]=>
  int(1952035200)
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
      bool(false)
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
      bool(false)
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
    ["keyUsage"]=>
    string(45) "Digital Signature, Certificate Sign, CRL Sign"
    ["basicConstraints"]=>
    string(7) "CA:TRUE"
    ["subjectKeyIdentifier"]=>
    string(59) "45:EB:A2:AF:F4:92:CB:82:31:2D:51:8B:A7:A7:21:9D:F3:6D:C8:0F"
    ["authorityKeyIdentifier"]=>
    string(66) "keyid:45:EB:A2:AF:F4:92:CB:82:31:2D:51:8B:A7:A7:21:9D:F3:6D:C8:0F
"
  }
  ["criticalExtensions"]=>
  array(2) {
    [0]=>
    string(8) "keyUsage"
    [1]=>
    string(16) "basicConstraints"
  }
}

