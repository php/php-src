--TEST--
Multiple declarations of the same static variable
--FILE--
<?php

static $a = 10;
static $a = 11;

?>
--EXPECTF--
Fatal error: Duplicate declaration of static variable $a in %s on line %d
