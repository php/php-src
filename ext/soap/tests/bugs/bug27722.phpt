--TEST--
Bug #27722 (Segfault on schema without targetNamespace)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
wsdl
--FILE--
<?php
$x = new SoapClient(dirname(__FILE__)."/bug27722.wsdl");
echo "ok\n";
?>
--EXPECT--
ok