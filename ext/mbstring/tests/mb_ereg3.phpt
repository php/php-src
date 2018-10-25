--TEST--
mb_ereg() returning matches
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
if (!function_exists("mb_ereg")) print "skip mb_ereg() not available";
?>
--FILE--
<?php

// Note: This test is identical to mb_ereg2.phpt, but using deprecated mbereg_* aliases.
// Deleted it once the deprecated aliases have been removed.

$a = -1; $b = -1; $c = -1;
mbereg($a, $b, $c);
var_dump($a, $b, $c);

mberegi($a, $b, $c);
var_dump($a, $b, $c);

mbereg_search_init($a, $b, $c);
var_dump($a, $b, $c);

echo "Done\n";
?>
--EXPECTF--
Deprecated: Function mbereg() is deprecated in %s on line %d
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}

Deprecated: Function mberegi() is deprecated in %s on line %d
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}

Deprecated: Function mbereg_search_init() is deprecated in %s on line %d

Warning: mbereg_search_init() expects parameter 3 to be string, array given in %s on line %d
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}
Done
