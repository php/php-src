--TEST--
GetImageSize() for xbm format
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_XBM")) {
		die("skip xbm file format is not available");
	}
?>
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array
 * Source code: ext/standard/image.c
 * Alias to functions:
 */

echo "*** Testing getimagesize() : xbm format ***\n";
var_dump(getimagesize(dirname(__FILE__) . "/75x50.xbm", $arr));
var_dump($arr);

?>
===DONE===
--EXPECT--
*** Testing getimagesize() : xbm format ***
array(5) {
  [0]=>
  int(75)
  [1]=>
  int(50)
  [2]=>
  int(16)
  [3]=>
  string(22) "width="75" height="50""
  ["mime"]=>
  string(9) "image/xbm"
}
array(0) {
}
===DONE===
