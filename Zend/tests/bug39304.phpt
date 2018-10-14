--TEST--
Bug #39304 (Segmentation fault with list unpacking of string offset)
--FILE--
<?php
  $s = "";
  list($a, $b) = $s[0];
  var_dump($a,$b);
?>
--EXPECTF--
Notice: Uninitialized string offset: 0 in %sbug39304.php on line %d
NULL
NULL
