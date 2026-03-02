--TEST--
Bug #75502 (Segmentation fault in zend_string_release)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=1
soap.wsdl_cache=2
--FILE--
<?php
/* The important part is that restriction>enumeration is used together with mem cache.
 * Reuse a WSDL file contains this. */
$client = new SoapClient(__DIR__."/bug29236.wsdl");
?>
===DONE===
--EXPECT--
===DONE===
