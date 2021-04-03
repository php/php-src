--TEST--
Bug #41337 (WSDL parsing doesn't ignore non soap bindings)
--EXTENSIONS--
soap
--FILE--
<?php
ini_set("soap.wsdl_cache_enabled",0);
$client = new SoapClient(__DIR__."/bug41337_2.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
