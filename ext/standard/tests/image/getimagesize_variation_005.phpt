--TEST--
Test getimagesize() function : basic functionality for shockwave-flash
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
echo "*** Testing getimagesize() : basic functionality ***\n";

var_dump( getimagesize(__DIR__."/test13pix.swf", $info) );
var_dump( $info );
?>
--EXPECT--
*** Testing getimagesize() : basic functionality ***
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
array(0) {
}
