--TEST--
NumberFormatter: rounding modes
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '69.0') < 0) die('skip for ICU >= 69.0'); ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(NumberFormatter::ROUND_CEILING);
var_dump(NumberFormatter::ROUND_FLOOR);
var_dump(NumberFormatter::ROUND_DOWN);
var_dump(NumberFormatter::ROUND_TOWARD_ZERO);
var_dump(NumberFormatter::ROUND_UP);
var_dump(NumberFormatter::ROUND_AWAY_FROM_ZERO);
var_dump(NumberFormatter::ROUND_HALFEVEN);
var_dump(NumberFormatter::ROUND_HALFODD);
var_dump(NumberFormatter::ROUND_HALFDOWN);
var_dump(NumberFormatter::ROUND_HALFUP);

var_dump(NumberFormatter::ROUND_DOWN === NumberFormatter::ROUND_TOWARD_ZERO);
var_dump(NumberFormatter::ROUND_UP === NumberFormatter::ROUND_AWAY_FROM_ZERO);
?>
--EXPECTF--
int(0)
int(1)
int(2)
int(2)
int(3)
int(3)
int(4)
int(8)
int(5)
int(6)
bool(true)
bool(true)
