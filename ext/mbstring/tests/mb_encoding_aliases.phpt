--TEST--
mb_encoding_aliases()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_encoding_aliases();
$list = mb_encoding_aliases("ASCII");
sort($list);
var_dump($list);
var_dump(mb_encoding_aliases("7bit"));
var_dump(mb_encoding_aliases("8bit"));
?>
--EXPECTF--
Warning: mb_encoding_aliases() expects exactly 1 parameter, 0 given in %s on line 2
array(10) {
  [0]=>
  string(14) "ANSI_X3.4-1968"
  [1]=>
  string(14) "ANSI_X3.4-1986"
  [2]=>
  string(6) "IBM367"
  [3]=>
  string(9) "ISO646-US"
  [4]=>
  string(16) "ISO_646.irv:1991"
  [5]=>
  string(8) "US-ASCII"
  [6]=>
  string(5) "cp367"
  [7]=>
  string(7) "csASCII"
  [8]=>
  string(8) "iso-ir-6"
  [9]=>
  string(2) "us"
}
array(0) {
}
array(1) {
  [0]=>
  string(6) "binary"
}
