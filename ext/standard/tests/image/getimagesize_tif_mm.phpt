--TEST--
GetImageSize() for tiff format with big-endian (aka Motorola, aka MM) ordering
--FILE--
<?php
echo "*** Testing getimagesize() : tiff_mm format ***\n";
var_dump(getimagesize(__DIR__ . "/2x2mm.tiff", $arr));
var_dump($arr);

?>
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
