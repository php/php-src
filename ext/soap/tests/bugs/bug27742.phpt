--TEST--
Bug #27742 (WDSL SOAP Parsing Schema bug)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--GET--
wsdl
--FILE--
<?php
$x = new SoapClient(dirname(__FILE__)."/bug27742.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
