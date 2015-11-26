--TEST--
Options type checks
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$x = openssl_pkey_new();
$csr = openssl_csr_new(["countryName" => "DE"], $x, ["x509_extensions" => 0xDEADBEEF]);
?>
DONE
--EXPECT--
DONE
