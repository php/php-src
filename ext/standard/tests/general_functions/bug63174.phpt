--TEST--
Random number generators should not accept a range that would lead to overflow
--FILE--
<?php
mt_srand(123456);
var_dump(mt_rand(-1,            getrandmax()));
var_dump(mt_rand(-getrandmax(), getrandmax()));

--EXPECTF--
Warning: mt_rand(): range of max(2147483647) minus min(-1) exceeds getrandmax() in %s on line %d
bool(false)

Warning: mt_rand(): range of max(2147483647) minus min(-2147483647) exceeds getrandmax() in %s on line %d
bool(false)

