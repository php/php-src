--TEST--
Test unixtojd() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--INI--
date.timezone=UTC
--FILE--
<?php
putenv('TZ=UTC');

var_dump(unixtojd(-1)) . PHP_EOL;
var_dump(unixtojd(false)) . PHP_EOL;
var_dump(unixtojd(null)) . PHP_EOL;
var_dump(unixtojd(time())) . PHP_EOL;
var_dump(unixtojd(PHP_INT_MAX)) . PHP_EOL;
?>
--EXPECTF--
bool(false)
int(%d)
int(%d)
int(%d)
bool(false)
