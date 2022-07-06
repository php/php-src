--TEST--
Bug #74022 PHP Fast CGI crashes when reading from a pfx file with valid password
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$pfx = __DIR__ . DIRECTORY_SEPARATOR . "bug74022.pfx";
$cert_store = file_get_contents($pfx);

var_dump(openssl_pkcs12_read($cert_store, $cert_info, "csos"));
var_dump(openssl_error_string());
?>
--EXPECT--
bool(true)
bool(false)
