--TEST--
Bug #36614 (Segfault when using Soap)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$lo_soap = new SoapClient(__DIR__."/bug36614.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
