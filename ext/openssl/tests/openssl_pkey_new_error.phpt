--TEST--
openssl_pkey_new() error tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
/* openssl_pkey_get_details() segfaults when getting the information
	from openssl_pkey_new() with an empty sub-array arg 		*/

$rsa = array("rsa" => array());
$dsa = array("dsa" => array());
$dh = array("dh" => array());

openssl_pkey_get_details(openssl_pkey_new($rsa));
openssl_pkey_get_details(openssl_pkey_new($dsa));
openssl_pkey_get_details(openssl_pkey_new($dh));
?>
--EXPECTF--

Warning: openssl_pkey_get_details() expects parameter 1 to be resource, boolean given in %s on line %d

Warning: openssl_pkey_get_details() expects parameter 1 to be resource, boolean given in %s on line %d

Warning: openssl_pkey_get_details() expects parameter 1 to be resource, boolean given in %s on line %d

