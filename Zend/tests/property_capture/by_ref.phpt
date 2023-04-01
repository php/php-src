--TEST--
Capturing properties by reference
--FILE--
<?php
$a = $b = $c = $d = $e = $f = $g = $h = 42;

$foo = new class use (
    &$a,
    &$b as $b_name,
    &$c as private,
    &$d as private $d_name,
    &$e as int,
    &$f as int $f_name,
    &$g as private int,
    &$h as private int $h_name,
) {};

var_dump($foo);

$a = 1; $b = 2; $c = 3; $d = 4; $e = 5; $f = 6; $g = 7; $h = 8;

var_dump($foo);
?>
--EXPECTF--
object(class@anonymous)#1 (8) {
  ["a"]=>
  &int(42)
  ["b_name"]=>
  &int(42)
  ["c":"class@anonymous":private]=>
  &int(42)
  ["d_name":"class@anonymous":private]=>
  &int(42)
  ["e"]=>
  &int(42)
  ["f_name"]=>
  &int(42)
  ["g":"class@anonymous":private]=>
  &int(42)
  ["h_name":"class@anonymous":private]=>
  &int(42)
}
object(class@anonymous)#1 (8) {
  ["a"]=>
  &int(1)
  ["b_name"]=>
  &int(2)
  ["c":"class@anonymous":private]=>
  &int(3)
  ["d_name":"class@anonymous":private]=>
  &int(4)
  ["e"]=>
  &int(5)
  ["f_name"]=>
  &int(6)
  ["g":"class@anonymous":private]=>
  &int(7)
  ["h_name":"class@anonymous":private]=>
  &int(8)
}
