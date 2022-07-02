--TEST--
mb_ereg() returning matches
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_ereg")) print "skip mb_ereg() not available";
?>
--FILE--
<?php

$a = -1; $b = -1; $c = -1;
mb_ereg($a, $b, $c);
var_dump($a, $b, $c);

mb_eregi($a, $b, $c);
var_dump($a, $b, $c);

echo "Done\n";
?>
--EXPECT--
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}
int(-1)
int(-1)
array(1) {
  [0]=>
  string(2) "-1"
}
Done
