--TEST--
GetImageSize()
--SKIPIF--
<?php
	require_once('skipif_imagetype.inc');
?>
--FILE--
<?php
	// Note: SWC requires zlib
	$dir = opendir(dirname(__FILE__)) or die('cannot open directory: '.dirname(__FILE__));
	$result = array();
	$files  = array();
	while (($file = readdir($dir)) !== FALSE) {
		if (preg_match('/^test.+pix\./',$file) && $file != "test13pix.swf") {
			$files[] = $file;
		}
	}
	closedir($dir);
	sort($files);
	foreach($files as $file) {
		$result[$file] = getimagesize(dirname(__FILE__)."/$file");
	}
	var_dump($result);
?>
--EXPECT--
array(16) {
  ["test-1pix.bmp"]=>
  array(6) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(1)
    ["type"]=>
    int(6)
    ["text"]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(24)
    ["mime"]=>
    string(14) "image/x-ms-bmp"
  }
  ["test12pix.webp"]=>
  array(6) {
    ["width"]=>
    int(4)
    ["height"]=>
    int(3)
    ["type"]=>
    int(18)
    ["text"]=>
    string(20) "width="4" height="3""
    ["bits"]=>
    int(8)
    ["mime"]=>
    string(10) "image/webp"
  }
  ["test1bpix.bmp"]=>
  array(6) {
    ["width"]=>
    int(500)
    ["height"]=>
    int(345)
    ["type"]=>
    int(6)
    ["text"]=>
    string(24) "width="500" height="345""
    ["bits"]=>
    int(32)
    ["mime"]=>
    string(14) "image/x-ms-bmp"
  }
  ["test1pix.bmp"]=>
  array(6) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(1)
    ["type"]=>
    int(6)
    ["text"]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(24)
    ["mime"]=>
    string(14) "image/x-ms-bmp"
  }
  ["test1pix.jp2"]=>
  array(7) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(1)
    ["type"]=>
    int(10)
    ["text"]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(8)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(9) "image/jp2"
  }
  ["test1pix.jpc"]=>
  array(7) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(1)
    ["type"]=>
    int(9)
    ["text"]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(8)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(24) "application/octet-stream"
  }
  ["test1pix.jpg"]=>
  array(7) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(1)
    ["type"]=>
    int(2)
    ["text"]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(8)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(10) "image/jpeg"
  }
  ["test2pix.gif"]=>
  array(7) {
    ["width"]=>
    int(2)
    ["height"]=>
    int(1)
    ["type"]=>
    int(1)
    ["text"]=>
    string(20) "width="2" height="1""
    ["bits"]=>
    int(1)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(9) "image/gif"
  }
  ["test3llpix.webp"]=>
  array(6) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(3)
    ["type"]=>
    int(18)
    ["text"]=>
    string(20) "width="1" height="3""
    ["bits"]=>
    int(8)
    ["mime"]=>
    string(10) "image/webp"
  }
  ["test3pix.webp"]=>
  array(6) {
    ["width"]=>
    int(1)
    ["height"]=>
    int(3)
    ["type"]=>
    int(18)
    ["text"]=>
    string(20) "width="1" height="3""
    ["bits"]=>
    int(8)
    ["mime"]=>
    string(10) "image/webp"
  }
  ["test4pix.gif"]=>
  array(7) {
    ["width"]=>
    int(4)
    ["height"]=>
    int(1)
    ["type"]=>
    int(1)
    ["text"]=>
    string(20) "width="4" height="1""
    ["bits"]=>
    int(2)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(9) "image/gif"
  }
  ["test4pix.iff"]=>
  array(6) {
    ["width"]=>
    int(4)
    ["height"]=>
    int(1)
    ["type"]=>
    int(14)
    ["text"]=>
    string(20) "width="4" height="1""
    ["bits"]=>
    int(4)
    ["mime"]=>
    string(9) "image/iff"
  }
  ["test4pix.png"]=>
  array(6) {
    ["width"]=>
    int(4)
    ["height"]=>
    int(1)
    ["type"]=>
    int(3)
    ["text"]=>
    string(20) "width="4" height="1""
    ["bits"]=>
    int(4)
    ["mime"]=>
    string(9) "image/png"
  }
  ["test4pix.psd"]=>
  array(5) {
    ["width"]=>
    int(4)
    ["height"]=>
    int(1)
    ["type"]=>
    int(5)
    ["text"]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(9) "image/psd"
  }
  ["test4pix.swf"]=>
  array(5) {
    ["width"]=>
    int(550)
    ["height"]=>
    int(400)
    ["type"]=>
    int(4)
    ["text"]=>
    string(24) "width="550" height="400""
    ["mime"]=>
    string(29) "application/x-shockwave-flash"
  }
  ["test4pix.tif"]=>
  array(5) {
    ["width"]=>
    int(4)
    ["height"]=>
    int(1)
    ["type"]=>
    int(7)
    ["text"]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(10) "image/tiff"
  }
}
