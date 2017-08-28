--TEST--
Invalid Fault code - if an empty string
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fault = new SoapFault("", "message");
?>
--EXPECTF--
Warning: SoapFault::SoapFault(): Invalid fault code in %s on line %d
