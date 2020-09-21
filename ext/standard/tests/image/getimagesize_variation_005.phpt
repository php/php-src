--TEST--
Test getimagesize() function : basic functionality for shockwave-flash
--SKIPIF--
<?php
    if (!defined("IMAGETYPE_SWC") || !extension_loaded('zlib')) {
        die("skip zlib extension is not available or SWC not supported");
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
