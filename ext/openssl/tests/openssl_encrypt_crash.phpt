--TEST--
openssl_encrypt() crash with old OpenSSL
--EXTENSIONS--
openssl
--FILE--
<?php
openssl_encrypt('', 'AES-128-CBC', 'foo');
var_dump("done");
?>
--EXPECTF--
Warning: openssl_encrypt(): Using an empty Initialization Vector (iv) is potentially insecure and not recommended in %s on line %d
string(4) "done"
