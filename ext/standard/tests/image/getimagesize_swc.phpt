--TEST--
GetImageSize() for compressed swf files
--SKIPIF--
<?php
	if (!defined("IMAGETYPE_SWC") || !extension_loaded('zlib')) {
		die("skip zlib extension is not avaliable");
	}
?>
--FILE--
<?php
	var_dump(getimagesize(dirname(__FILE__) . "/test13pix.swf"));
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
