--TEST--
Bug #44686 (SOAP-ERROR: Parsing WSDL with references)
--SKIPIF--
<?php require_once 'skipif.inc'; ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
new SoapClient(dirname(__FILE__) . "/bug44686.wsdl");
echo "ok\n";
?>
--EXPECT--
ok
