--TEST--
Bug #71848 (getimagesize with $imageinfo returns false)
--FILE--
<?php
var_dump(getimagesize(__DIR__ . '/bug71848.jpg', $info));
var_dump(array_keys($info));
?>
===DONE===
--EXPECT--
array(7) {
  [0]=>
  int(8)
  [1]=>
  int(8)
  [2]=>
  int(2)
  [3]=>
  string(20) "width="8" height="8""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(10) "image/jpeg"
}
array(2) {
  [0]=>
  string(4) "APP0"
  [1]=>
  string(4) "APP5"
}
===DONE===
