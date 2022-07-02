--TEST--
Bug #27722 (Segfault on schema without targetNamespace)
--EXTENSIONS--
soap
--GET--
wsdl
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
$x = new SoapClient(__DIR__."/bug27722.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
