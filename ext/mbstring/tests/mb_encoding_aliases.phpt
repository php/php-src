--TEST--
mb_encoding_aliases()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
$list = mb_encoding_aliases("ASCII");
sort($list);
var_dump($list);
var_dump(mb_encoding_aliases("7bit"));
var_dump(mb_encoding_aliases("8bit"));
var_dump(mb_encoding_aliases("BAD"));
?>
--EXPECTF--
array(11) {
  [0]=>
  string(14) "ANSI_X3.4-1968"
  [1]=>
  string(14) "ANSI_X3.4-1986"
  [2]=>
  string(7) "IBM-367"
  [3]=>
  string(6) "IBM367"
  [4]=>
  string(9) "ISO646-US"
  [5]=>
  string(16) "ISO_646.irv:1991"
  [6]=>
  string(8) "US-ASCII"
  [7]=>
  string(5) "cp367"
  [8]=>
  string(7) "csASCII"
  [9]=>
  string(8) "iso-ir-6"
  [10]=>
  string(2) "us"
}
array(0) {
}
array(1) {
  [0]=>
  string(6) "binary"
}

Warning: mb_encoding_aliases(): Unknown encoding "BAD" in %s on line %d
bool(false)
