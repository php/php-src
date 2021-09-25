--TEST--
Bug #44686 (SOAP-ERROR: Parsing WSDL with references)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
new SoapClient(__DIR__ . "/bug44686.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
