--TEST--
CMP_LT, CMP_EQ and CMP_GT ordering constants
--FILE--
<?php

echo "Check values", PHP_EOL;
var_dump(CMP_LT, CMP_EQ, CMP_GT);

echo "Make sure they match <=> and strcmp() return values", PHP_EOL;
var_dump((1 <=> 2) === CMP_LT);
var_dump((1 <=> 1) === CMP_EQ);
var_dump((2 <=> 1) === CMP_GT);
var_dump(strcmp("a", "b") === CMP_LT);
var_dump(strcmp("a", "a") === CMP_EQ);
var_dump(strcmp("b", "a") === CMP_GT);

echo "Make sure they're case-sensitive and lowercase variants aren't defined", PHP_EOL;
var_dump(defined("cmp_lt"));
var_dump(defined("cmp_eq"));
var_dump(defined("cmp_gt"));

?>
--EXPECTF--
Check values
int(-1)
int(0)
int(1)
Make sure they match <=> and strcmp() return values
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Make sure they're case-sensitive and lowercase variants aren't defined
bool(false)
bool(false)
bool(false)
