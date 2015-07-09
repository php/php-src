--TEST--
Random number generators should not accept a range that would lead to overflow
--FILE--
<?php
mt_srand(123456);
var_dump(mt_rand(-1,            getrandmax()));
var_dump(mt_rand(-getrandmax(), getrandmax()));
var_dump(mt_rand(-getrandmax(), getrandmax()));
var_dump(mt_rand(PHP_INT_MIN,   PHP_INT_MAX));

--EXPECTF--
Warning: mt_rand(): range of max(%i) minus min(%i) exceeds getrandmax() in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) exceeds getrandmax() in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) exceeds getrandmax() in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) exceeds getrandmax() in %s on line %d
bool(false)
