--TEST--
Bug #39304 (Segmentation fault with list unpacking of string offset)
--FILE--
<?php 
  $s = "";
  list($a, $b) = $s[0];
?>
--EXPECTF--
Notice: Uninitialized string offset: 0 in %sbug39304.php on line 3

Fatal error: Cannot use string offset as an array in %sbug39304.php on line 3
