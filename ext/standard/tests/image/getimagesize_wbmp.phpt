--TEST--
GetImageSize() for wbmp format
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array
 * Source code: ext/standard/image.c
 * Alias to functions:
 */

echo "*** Testing getimagesize() : wbmp format ***\n";
var_dump(getimagesize(__DIR__ . "/75x50.wbmp", $arr));
var_dump($arr);

?>
===DONE===
--EXPECT--
*** Testing getimagesize() : wbmp format ***
array(5) {
  [0]=>
  int(75)
  [1]=>
  int(50)
  [2]=>
  int(15)
  [3]=>
  string(22) "width="75" height="50""
  ["mime"]=>
  string(18) "image/vnd.wap.wbmp"
}
array(0) {
}
===DONE===
