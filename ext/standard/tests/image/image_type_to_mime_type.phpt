--TEST--
image_type_to_mime_type()
--SKIPIF--
<?php 
	if (!function_exists('image_type_to_mime_type')) die('skip image_type_to_mime_type() not available'); 
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
		$result[$file] = image_type_to_mime_type($result[$file][2]);
	}
	var_dump($result);
?>
--EXPECT--
array(11) {
  ["test1pix.bmp"]=>
  string(9) "image/bmp"
  ["test1pix.jp2"]=>
  string(9) "image/jp2"
  ["test1pix.jpc"]=>
  string(24) "application/octet-stream"
  ["test1pix.jpg"]=>
  string(10) "image/jpeg"
  ["test2pix.gif"]=>
  string(9) "image/gif"
  ["test4pix.gif"]=>
  string(9) "image/gif"
  ["test4pix.iff"]=>
  string(9) "image/iff"
  ["test4pix.png"]=>
  string(9) "image/png"
  ["test4pix.psd"]=>
  string(9) "image/psd"
  ["test4pix.swf"]=>
  string(29) "application/x-shockwave-flash"
  ["test4pix.tif"]=>
  string(10) "image/tiff"
}