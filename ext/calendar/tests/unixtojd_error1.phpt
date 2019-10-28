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

try {
    unixtojd(-1);
} catch (ValueError $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
var_dump(unixtojd(false)) . PHP_EOL;
var_dump(unixtojd(null)) . PHP_EOL;
var_dump(unixtojd(time())) . PHP_EOL;
?>
--EXPECTF--
timestamp must not be negative
int(%d)
int(%d)
int(%d)
