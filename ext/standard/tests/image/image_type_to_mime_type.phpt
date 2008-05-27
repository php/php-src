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
  [u"test1pix.bmp"]=>
  unicode(9) "image/bmp"
  [u"test1pix.jp2"]=>
  unicode(9) "image/jp2"
  [u"test1pix.jpc"]=>
  unicode(24) "application/octet-stream"
  [u"test1pix.jpg"]=>
  unicode(10) "image/jpeg"
  [u"test2pix.gif"]=>
  unicode(9) "image/gif"
  [u"test4pix.gif"]=>
  unicode(9) "image/gif"
  [u"test4pix.iff"]=>
  unicode(9) "image/iff"
  [u"test4pix.png"]=>
  unicode(9) "image/png"
  [u"test4pix.psd"]=>
  unicode(9) "image/psd"
  [u"test4pix.swf"]=>
  unicode(29) "application/x-shockwave-flash"
  [u"test4pix.tif"]=>
  unicode(10) "image/tiff"
}
