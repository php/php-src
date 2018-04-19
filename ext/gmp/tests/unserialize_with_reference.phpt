--TEST--
Unserialize GMP instance with internal reference to itself
--SKIPIF--
<?php if (!extension_loaded('gmp')) die('skip gmp extension not loaded'); ?>
--FILE--
<?php
$s = 'C:3:"GMP":23:{s:1:"2";a:1:{i:46;R:1;}}';
var_dump(unserialize($s));
?>
--EXPECT--
object(GMP)#1 (2) {
  [46]=>
  *RECURSION*
  ["num"]=>
  string(1) "2"
}
