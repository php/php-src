--TEST--
GetImageSize() for avif using monochrome format
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array
 * Source code: ext/standard/image.c
 * Alias to functions:
 */

echo "*** Testing getimagesize() : avif using monochrome format ***\n";
var_dump(getimagesize(__DIR__ . "/monochrome.avif", $arr));
var_dump($arr);
?>
--EXPECT--
*** Testing getimagesize() : avif using monochrome format ***
array(7) {
  [0]=>
  int(8)
  [1]=>
  int(1)
  [2]=>
  int(19)
  [3]=>
  string(20) "width="8" height="1""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(1)
  ["mime"]=>
  string(10) "image/avif"
}
array(0) {
}
