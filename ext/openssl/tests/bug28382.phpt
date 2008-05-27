--TEST--
Bug #28382 (openssl_x509_parse extensions support)
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip"); 
if (OPENSSL_VERSION_NUMBER<0x009070af) die("skip");
?>
--FILE--
<?php
$cert = file_get_contents(dirname(__FILE__) . "/bug28382cert.txt");
$ext = openssl_x509_parse($cert);
var_dump($ext['extensions']);
?>
--EXPECTF--
array(11) {
  [u"basicConstraints"]=>
  unicode(8) "CA:FALSE"
  [u"nsComment"]=>
  unicode(38) "For Grid use only; request tag userTag"
  [u"nsCertType"]=>
  unicode(30) "SSL Client, SSL Server, S/MIME"
  [u"crlDistributionPoints"]=>
  unicode(51) "URI:http://mobile.blue-software.ro:90/ca/crl.shtml
"
  [u"nsCaPolicyUrl"]=>
  unicode(38) "http://mobile.blue-software.ro:90/pub/"
  [u"subjectAltName"]=>
  unicode(28) "email:sergiu@bluesoftware.ro"
  [u"subjectKeyIdentifier"]=>
  unicode(59) "B0:A7:FF:F9:41:15:DE:23:39:BD:DD:31:0F:97:A0:B2:A2:74:E0:FC"
  [u"authorityKeyIdentifier"]=>
  unicode(115) "DirName:/C=RO/ST=Romania/L=Craiova/O=Sergiu/OU=Sergiu SRL/CN=Sergiu CA/emailAddress=n_sergiu@hotmail.com
serial:00
"
  [u"keyUsage"]=>
  unicode(71) "Digital Signature, Non Repudiation, Key Encipherment, Data Encipherment"
  [u"nsBaseUrl"]=>
  unicode(20) "http://62.231.98.52/"
  [u"1.2.3.4"]=>
  string(4) "%s"
}
