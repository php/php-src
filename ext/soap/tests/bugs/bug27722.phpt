--TEST--
Bug #27722 (Segfault on schema without targetNamespace)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
wsdl
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$x = new SoapClient(dirname(__FILE__)."/bug27722.wsdl");
echo "ok\n";
?>
--EXPECT--
ok