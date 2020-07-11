--TEST--
Bug #65689 (GeneralizedTime format parsing)
--SKIPIF--
<?php if (!extension_loaded("openssl")) die("skip"); ?>
--FILE--
<?php
$crt = substr(__FILE__, 0, -4).'.crt';
$info = openssl_x509_parse("file://$crt");
var_dump($info["validFrom"], $info["validFrom_time_t"], $info["validTo"], $info["validTo_time_t"]);
?>
Done
--EXPECTF--
string(15) "20090303125318Z"
int(12360%d)
string(15) "20240303125318Z"
int(17094%d)
Done
