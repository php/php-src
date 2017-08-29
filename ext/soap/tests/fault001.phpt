--TEST--
is_soap_fault 1: test against null
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo (int) is_soap_fault(null);
?>
--EXPECT--
0
