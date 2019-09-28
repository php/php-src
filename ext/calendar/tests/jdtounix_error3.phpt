--TEST--
jdtounix() - when parameter is not an integer
--CREDITS--
Danilo Terra - <danilo_terra@live.com>
--INI--
date.timezone=UTC
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
//float
var_dump(jdtounix(M_PI)) . PHP_EOL;
// null
var_dump(jdtounix(null)) . PHP_EOL;
?>
--EXPECTF--
bool(false)
bool(false)