--TEST--
unixtojd() - when function receives more than 1 parameter
--CREDITS--
dterra - <danilo_terra@live.com>
# Hacktoberfest 2019
--SKIPIF--
<?php include 'skipif.inc'; ?>
--INI--
date.timezone=UTC
--FILE--
<?php
putenv('TZ=UTC');
var_dump(unixtojd(400, 400)) . PHP_EOL;
?>
--EXPECTF--
Warning: unixtojd() expects at most 1 parameter, 2 given in %s on line %d
NULL
