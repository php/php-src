--TEST--
jdtounix() - when parameter is not an integer
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--INI--
date.timezone=UTC
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
// string
var_dump(jdtounix('string')) . PHP_EOL;
// array
var_dump(jdtounix([])) . PHP_EOL;
// object
var_dump(jdtounix(new \stdClass()));
// resource
var_dump(jdtounix(fopen('php://stdin', 'r')));
?>
--EXPECTF--
Warning: jdtounix() expects parameter 1 to be int, string given in %s on line %d
NULL

Warning: jdtounix() expects parameter 1 to be int, array given in %s on line %d
NULL

Warning: jdtounix() expects parameter 1 to be int, object given in %s on line %d
NULL

Warning: jdtounix() expects parameter 1 to be int, resource given in %s on line %d
NULL