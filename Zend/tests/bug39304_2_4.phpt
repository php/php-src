--TEST--
Bug #39304 (Segmentation fault with list unpacking of string offset)
--SKIPIF--
<?php if (version_compare(zend_version(), '2.4.0', '<')) die('skip ZendEngine 2.4 needed'); ?>
--FILE--
<?php 
  $s = "";
  list($a, $b) = $s[0];
  var_dump($a,$b);
?>
--EXPECTF--
Notice: Uninitialized string offset: 0 in %sbug39304_2_4.php on line %d
NULL
NULL
