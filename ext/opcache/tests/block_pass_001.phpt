--TEST--
Block pass: Bugs in BOOL/QM_ASSIGN elision
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
(bool) (true ? $x : $y);
(bool) (true ? new StdClass : null);
(bool) new StdClass;
?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
