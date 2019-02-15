--TEST--
Bug #29061 (soap extension segfaults)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(dirname(__FILE__)."/bug29061.wsdl", array("exceptions"=>0));
$client->getQuote("ibm");
echo "ok\n";
?>
--EXPECT--
ok
