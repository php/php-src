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
array(11) {
  [u"test1pix.bmp"]=>
  array(6) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(6)
    [3]=>
    unicode(20) "width="1" height="1""
    [u"bits"]=>
    int(24)
    [u"mime"]=>
    unicode(9) "image/bmp"
  }
  [u"test1pix.jp2"]=>
  array(7) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(10)
    [3]=>
    unicode(20) "width="1" height="1""
    [u"bits"]=>
    int(8)
    [u"channels"]=>
    int(3)
    [u"mime"]=>
    unicode(9) "image/jp2"
  }
  [u"test1pix.jpc"]=>
  array(7) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(9)
    [3]=>
    unicode(20) "width="1" height="1""
    [u"bits"]=>
    int(8)
    [u"channels"]=>
    int(3)
    [u"mime"]=>
    unicode(24) "application/octet-stream"
  }
  [u"test1pix.jpg"]=>
  array(7) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    unicode(20) "width="1" height="1""
    [u"bits"]=>
    int(8)
    [u"channels"]=>
    int(3)
    [u"mime"]=>
    unicode(10) "image/jpeg"
  }
  [u"test2pix.gif"]=>
  array(7) {
    [0]=>
    int(2)
    [1]=>
    int(1)
    [2]=>
    int(1)
    [3]=>
    unicode(20) "width="2" height="1""
    [u"bits"]=>
    int(1)
    [u"channels"]=>
    int(3)
    [u"mime"]=>
    unicode(9) "image/gif"
  }
  [u"test4pix.gif"]=>
  array(7) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(1)
    [3]=>
    unicode(20) "width="4" height="1""
    [u"bits"]=>
    int(2)
    [u"channels"]=>
    int(3)
    [u"mime"]=>
    unicode(9) "image/gif"
  }
  [u"test4pix.iff"]=>
  array(6) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(14)
    [3]=>
    unicode(20) "width="4" height="1""
    [u"bits"]=>
    int(4)
    [u"mime"]=>
    unicode(9) "image/iff"
  }
  [u"test4pix.png"]=>
  array(6) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(3)
    [3]=>
    unicode(20) "width="4" height="1""
    [u"bits"]=>
    int(4)
    [u"mime"]=>
    unicode(9) "image/png"
  }
  [u"test4pix.psd"]=>
  array(5) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(5)
    [3]=>
    unicode(20) "width="4" height="1""
    [u"mime"]=>
    unicode(9) "image/psd"
  }
  [u"test4pix.swf"]=>
  array(5) {
    [0]=>
    int(550)
    [1]=>
    int(400)
    [2]=>
    int(4)
    [3]=>
    unicode(24) "width="550" height="400""
    [u"mime"]=>
    unicode(29) "application/x-shockwave-flash"
  }
  [u"test4pix.tif"]=>
  array(5) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(7)
    [3]=>
    unicode(20) "width="4" height="1""
    [u"mime"]=>
    unicode(10) "image/tiff"
  }
}
