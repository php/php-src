--TEST--
GetImageSize() for tiff format with big-endian (aka Motorola, aka MM) ordering
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_TIFF_MM")) {
		die("skip tiff_mm file format is not available");
	}
?>
--FILE--
<?php
/* Prototype  : proto array getimagesize(string imagefile [, array info])
 * Description: Get the size of an image as 4-element array 
 * Source code: ext/standard/image.c
 * Alias to functions: 
 */

echo "*** Testing getimagesize() : tiff_mm format ***\n";
var_dump(getimagesize(dirname(__FILE__) . "/2x2mm.tif", $arr));
var_dump($arr);

?>
===DONE===
--EXPECT--
*** Testing getimagesize() : tiff_mm format ***
array(5) {
  [0]=>
  int(2)
  [1]=>
  int(2)
  [2]=>
  int(8)
  [3]=>
  string(20) "width="2" height="2""
  ["mime"]=>
  string(10) "image/tiff"
}
array(0) {
}
===DONE===