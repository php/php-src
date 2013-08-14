--TEST--
Bug #64802: openssl_x509_parse fails to parse subject properly in some cases
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$cert = file_get_contents(__DIR__.'/bug64802.pem');
$r = openssl_x509_parse($cert,$use_short_names=true);
var_dump( count($r['subject'])>1 );
?>
--EXPECTF--
bool(true)
