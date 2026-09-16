--TEST--
openssl_pkey_new() error tests
--EXTENSIONS--
openssl
--FILE--
<?php
/* openssl_pkey_get_details() segfaults when getting the information
    from openssl_pkey_new() with an empty sub-array arg 		*/

$rsa = array("rsa" => array());
$dsa = array("dsa" => array());
$dh = array("dh" => array());

try {
    openssl_pkey_get_details(openssl_pkey_new($rsa));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    openssl_pkey_get_details(openssl_pkey_new($dsa));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    openssl_pkey_get_details(openssl_pkey_new($dh));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: openssl_pkey_get_details(): Argument #1 ($key) must be of type OpenSSLAsymmetricKey, false given
TypeError: openssl_pkey_get_details(): Argument #1 ($key) must be of type OpenSSLAsymmetricKey, false given
TypeError: openssl_pkey_get_details(): Argument #1 ($key) must be of type OpenSSLAsymmetricKey, false given
