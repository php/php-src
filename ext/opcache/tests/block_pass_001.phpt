--TEST--
Block pass: Bugs in BOOL/QM_ASSIGN elision
--EXTENSIONS--
opcache
--FILE--
<?php
(bool) (true ? $x : $y);
(bool) (true ? new stdClass : null);
(bool) new stdClass;
?>
--EXPECTF--
Warning: Undefined variable $x in %s on line %d
