--TEST--
Bug #70052 (getimagesize() fails for very large and very small WBMP)
--FILE--
<?php
var_dump(getimagesize(__DIR__ . '/bug70052_1.wbmp'));
var_dump(getimagesize(__DIR__ . '/bug70052_2.wbmp'));
?>
--EXPECT--
bool(false)
array(5) {
  ["width"]=>
  int(3)
  ["height"]=>
  int(3)
  ["type"]=>
  int(15)
  ["text"]=>
  string(20) "width="3" height="3""
  ["mime"]=>
  string(18) "image/vnd.wap.wbmp"
}
