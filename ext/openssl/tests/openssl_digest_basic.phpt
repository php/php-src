--TEST--
openssl_digest() basic test
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$data = "openssl_digest() basic test";
$method = "md5";
$method2 = "sha1";

var_dump(openssl_digest($data, $method));
var_dump(openssl_digest($data, $method2));
?>
--EXPECT--
string(32) "f0045b6c41d9ec835cb8948c7fec4955"
string(40) "aa6e750fef05c2414c18860ad31f2c35e79bf3dc"
