--TEST--
033: Import statement with non-compound name
--FILE--
<?php
use A;
use \B;
--EXPECTF--
Warning: The use statement with non-compound name A has no effect in %s on line %d

Warning: The use statement with non-compound name B has no effect in %s on line %d
