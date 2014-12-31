--TEST--
Bug #39304 (Segmentation fault with list unpacking of string offset)
--FILE--
<?php 
  $s = "";
  list($a, $b) = $s[0];
echo "I am alive";
?>
--EXPECTF--
Notice: Uninitialized string offset: 0 in %sbug39304.php on line %d
I am alive

