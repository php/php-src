--TEST--
Assign coalesce: "$a[0] ??= $a" should evaluate the right $a first
--FILE--
<?php
$a[0] ??= $a;
var_dump($a);
?>
--EXPECTF--
Warning: Undefined variable $a (this will become an error in PHP 9.0) in %sassign_coalesce_007.php on line 2
array(1) {
  [0]=>
  NULL
}
