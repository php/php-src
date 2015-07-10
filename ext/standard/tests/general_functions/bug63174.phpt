--TEST--
Random number generators should not accept a range that would lead to overflow
--FILE--
<?php
// the biggest range we can accommodate, based on RAND_RANGE scaling algo
// 32bit: $upper == getrandmax()       // since we use 32-bit MT algo
// 64bit: getrandmax() < PHP_INT_MAX   // since we allow scaling into 64-bit range
$lower = PHP_INT_MIN;
$upper = PHP_INT_MAX;

// now run some tests at the edge of that range, all should pass
var_dump(mt_rand( 1,     $upper));    // just inside range
var_dump(mt_rand( 0,     $upper));    // just at range
var_dump(mt_rand(-1,     $upper-1));  // just at range, different scale
var_dump(mt_rand($lower, -1));        // just inside range

// now run some tests outside that range, we should get failures
var_dump(mt_rand(-1,      $upper)); // one too big
var_dump(mt_rand($lower,  1));      // also one too big
var_dump(mt_rand($lower,  $upper)); // $upper - $lower > PHP_INT_MAX
var_dump(mt_rand(-$upper, $upper)); // double your pleasure

// common idioms that should also never work
var_dump(mt_rand(PHP_INT_MIN, PHP_INT_MAX));

--EXPECTF--
int(%i)
int(%i)
int(%i)
int(%i)

Warning: mt_rand(): range of max(%i) minus min(%i) would overflow in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) would overflow in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) would overflow in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) would overflow in %s on line %d
bool(false)

Warning: mt_rand(): range of max(%i) minus min(%i) would overflow in %s on line %d
bool(false)
