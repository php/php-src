--TEST--
Test cal_from_jd() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
var_dump(cal_from_jd(1748326, -1));
?>
--EXPECTF--
Warning: cal_from_jd(): invalid calendar ID -1 in %s on line %d
bool(false)
