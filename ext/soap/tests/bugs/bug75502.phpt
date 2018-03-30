--TEST--
Bug #75502 (Segmentation fault in zend_string_release)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=1
soap.wsdl_cache=2
--FILE--
<?php
/* The important part is that restriction>enumeration is used together with mem cache.
 * Reuse a WSDL file contains this. */
$client = new SoapClient(dirname(__FILE__)."/bug29236.wsdl");
?>
===DONE===
--EXPECT--
===DONE===
