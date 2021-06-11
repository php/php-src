--TEST--
GetImageSize() for compressed swf files
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (!defined("IMAGETYPE_SWC")) {
    die("skip IMAGETYPE_SWC is not available");
}
?>
--FILE--
<?php
    var_dump(getimagesize(__DIR__ . "/test13pix.swf"));
?>
--EXPECT--
array(5) {
  [0]=>
  int(550)
  [1]=>
  int(400)
  [2]=>
  int(13)
  [3]=>
  string(24) "width="550" height="400""
  ["mime"]=>
  string(29) "application/x-shockwave-flash"
}
