--TEST--
Block pass: Bugs in BOOL/QM_ASSIGN elision
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
(bool) (true ? $x : $y);
(bool) (true ? new stdClass : null);
(bool) new stdClass;
?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
