--TEST--
is_soap_fault 1: test against null
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
var_dump(is_soap_fault(null));
$fault = new SoapFault("code", "message");
var_dump(is_soap_fault($fault));
?>
--EXPECT--
bool(false)
bool(true)
