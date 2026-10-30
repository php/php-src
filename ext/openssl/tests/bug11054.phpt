--TEST--
Bug #11054: Calling with a PEM public key results in error
--EXTENSIONS--
openssl
--FILE--
<?php

$key_file_name = __DIR__ . '/bug11054.pem';
$key_content = file_get_contents($key_file_name);
openssl_get_publickey($key_content);
var_dump(openssl_error_string());

?>
--EXPECT--
bool(false)
