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
		if (preg_match('/^test.+pix\./',$file)) {
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
  ["test13pix.swf"]=>
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
  ["test1pix.bmp"]=>
  array(6) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(6)
    [3]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(24)
    ["mime"]=>
    string(9) "image/bmp"
  }
  ["test1pix.jpc"]=>
  array(6) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(9)
    [3]=>
    string(20) "width="1" height="1""
    ["bits"]=>
    int(3)
    ["mime"]=>
    string(10) "image/jpeg"
  }
  ["test1pix.jpg"]=>
  array(7) {
    [0]=>
    int(1)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
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
    [0]=>
    int(2)
    [1]=>
    int(1)
    [2]=>
    int(1)
    [3]=>
    string(20) "width="2" height="1""
    ["bits"]=>
    int(1)
    ["channels"]=>
    int(3)
    ["mime"]=>
    string(9) "image/gif"
  }
  ["test4pix.gif"]=>
  array(7) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(1)
    [3]=>
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
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(14)
    [3]=>
    string(20) "width="4" height="1""
    ["bits"]=>
    int(4)
    ["mime"]=>
    string(9) "image/iff"
  }
  ["test4pix.png"]=>
  array(5) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(3)
    [3]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(9) "image/png"
  }
  ["test4pix.psd"]=>
  array(5) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(5)
    [3]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(9) "image/psd"
  }
  ["test4pix.swf"]=>
  array(5) {
    [0]=>
    int(550)
    [1]=>
    int(400)
    [2]=>
    int(4)
    [3]=>
    string(24) "width="550" height="400""
    ["mime"]=>
    string(29) "application/x-shockwave-flash"
  }
  ["test4pix.tif"]=>
  array(5) {
    [0]=>
    int(4)
    [1]=>
    int(1)
    [2]=>
    int(7)
    [3]=>
    string(20) "width="4" height="1""
    ["mime"]=>
    string(10) "image/tiff"
  }
}
