--TEST--
imagegetclip() - basic functionality
--SKIPIF--
<?php
if (!extension_loaded('gd')) die('skip ext/gd required');
?>
--FILE--
<?php
$im = imagecreate(10, 10);
echo '=== original ===', PHP_EOL;
var_dump(imagegetclip($im));
imagesetclip($im, 1,2, 3,4);
echo '=== after imagesetclip() ===', PHP_EOL;
var_dump(imagegetclip($im));
?>
--EXPECT--
=== original ===
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(9)
  [3]=>
  int(9)
}
=== after imagesetclip() ===
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
