--TEST--
Bug #29061 (soap extension segfaults)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$client = new SoapClient(__DIR__."/bug29061.wsdl", array("exceptions"=>0));
$client->getQuote("ibm");
echo "ok\n";
?>
--EXPECT--
ok
