--TEST--
GH-20201 (AVIF images misdetected as HEIF after introducing HEIF support in getimagesize())
--FILE--
<?php
var_dump(getimagesize(__DIR__ . '/gh20201.avif'));
?>
--EXPECT--
array(9) {
  [0]=>
  int(8)
  [1]=>
  int(8)
  [2]=>
  int(19)
  [3]=>
  string(20) "width="8" height="8""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(10) "image/avif"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
