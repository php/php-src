--TEST--
GetImageSize() for compressed swf files
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_SWC") || !extension_loaded('zlib')) {
		die("skip zlib extension is not available");
	}
?>
--FILE--
<?php
	var_dump(getimagesize(dirname(__FILE__) . "/test13pix.swf"));
?>
--EXPECT--
array(5) {
  ["width"]=>
  int(550)
  ["height"]=>
  int(400)
  ["type"]=>
  int(13)
  ["text"]=>
  string(24) "width="550" height="400""
  ["mime"]=>
  string(29) "application/x-shockwave-flash"
}
