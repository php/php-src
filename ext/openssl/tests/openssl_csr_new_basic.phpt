--TEST--
openssl_csr_new() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php

$a = 1;
var_dump(openssl_csr_new(1,$a));
var_dump(openssl_csr_new(1,$a,1,1));
$a = array();

$conf = array('config' => dirname(__FILE__) . DIRECTORY_SEPARATOR . 'openssl.cnf');
var_dump(openssl_csr_new(array(), $a, $conf, array()));

// this leaks
$a = array(1,2);
$b = array(1,2);
var_dump(openssl_csr_new($a, $b, $conf));

// options type check
$x = openssl_pkey_new($conf);
var_dump(openssl_csr_new(["countryName" => "DE"], $x, $conf + ["x509_extensions" => 0xDEADBEEF]));


echo "Done\n";
?>
--EXPECTF--
Warning: openssl_csr_new() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: openssl_csr_new() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: openssl_csr_new(): key array must be of the form array(0 => key, 1 => phrase) in %s on line %d

Warning: openssl_csr_new(): add1_attr_by_txt challengePassword_min -> 4 (failed; check error queue and value of string_mask OpenSSL option if illegal characters are reported) in %s on line %d
bool(false)
resource(%d) of type (OpenSSL X.509 CSR)
resource(%d) of type (OpenSSL X.509 CSR)
Done
