--TEST--
is_soap_fault 2: test against constructed SoapFault instance.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fault = new SoapFault("code", "message");
echo (int) is_soap_fault($fault);
?>
--EXPECT--
1
