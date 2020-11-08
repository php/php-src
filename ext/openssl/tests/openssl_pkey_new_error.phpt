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

try {
    openssl_pkey_get_details(openssl_pkey_new($rsa));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    openssl_pkey_get_details(openssl_pkey_new($dsa));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    openssl_pkey_get_details(openssl_pkey_new($dh));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
openssl_pkey_get_details(): Argument #1 ($key) must be of type OpenSSLAsymmetricKey, bool given
openssl_pkey_get_details(): Argument #1 ($key) must be of type OpenSSLAsymmetricKey, bool given
openssl_pkey_get_details(): Argument #1 ($key) must be of type OpenSSLAsymmetricKey, bool given
