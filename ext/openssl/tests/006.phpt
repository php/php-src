--TEST--
openssl_pkey_new() with an empty sub-array arg generates a malformed resource
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
/* openssl_pkey_get_details() segfaults when getting the information
	from openssl_pkey_new() with an empty sub-array arg 		*/

$rsa = array(b"rsa" => array());
$dsa = array(b"dsa" => array());
$dh = array(b"dh" => array());

openssl_pkey_get_details(openssl_pkey_new($rsa));
openssl_pkey_get_details(openssl_pkey_new($dsa));
openssl_pkey_get_details(openssl_pkey_new($dh));
?>
--EXPECTF--

Warning: openssl_pkey_get_details() expects parameter 1 to be resource, boolean given in %s on line %d

Warning: openssl_pkey_get_details() expects parameter 1 to be resource, boolean given in %s on line %d

Warning: openssl_pkey_get_details() expects parameter 1 to be resource, boolean given in %s on line %d

