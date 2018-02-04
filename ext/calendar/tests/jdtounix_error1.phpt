--TEST--
Test jdtounix() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--INI--
date.timezone=UTC
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
var_dump(jdtounix(2440579)) . PHP_EOL;
?>
--EXPECT--
bool(false)