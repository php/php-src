--TEST--
GetImageSize() for ico format with 256px height
--FILE--
<?php
echo "*** Testing getimagesize() : 256px ico ***\n";
var_dump(getimagesize(__DIR__ . "/32x256.ico"));

?>
===DONE===
--EXPECT--
*** Testing getimagesize() : 256px ico ***
array(6) {
  [0]=>
  int(32)
  [1]=>
  int(256)
  [2]=>
  int(17)
  [3]=>
  string(23) "width="32" height="256""
  ["bits"]=>
  int(8)
  ["mime"]=>
  string(24) "image/vnd.microsoft.icon"
}
===DONE===
