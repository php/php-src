--TEST--
Bug #65689 (GeneralizedTime format parsing)
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip"); 
?>
--FILE--
<?php
$crt = substr(__FILE__, 0, -4).'.crt';
$info = openssl_x509_parse("file://$crt");
var_dump($info["validFrom"], $info["validFrom_time_t"], $info["validTo"], $info["validTo_time_t"]);
?>
Done
--EXPECTF--
string(15) "20090303125318Z"
int(1236084798)
string(15) "20240303125318Z"
int(1709470398)
Done
