--TEST--
Bug #27742 (WDSL SOAP Parsing Schema bug)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
wsdl
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$x = new SoapClient(__DIR__."/bug27742.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
