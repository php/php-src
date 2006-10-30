--TEST--
Bug #39304 (Segmentation fault with list unpacking of string offset)
--FILE--
<?php 
  $s = "";
  list($a, $b) = $s[0];
?>
--EXPECTF--
Fatal error: Cannot use string offset as an array in %sbug39304.php on line 3
