--TEST--
GetImageSize() with 384x385 pixels
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
		if (preg_match('/^384x385\./',$file)) {
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
array(1) {
  ["384x385.png"]=>
  array(6) {
    ["width"]=>
    int(384)
    ["height"]=>
    int(385)
    ["type"]=>
    int(3)
    ["text"]=>
    string(24) "width="384" height="385""
    ["bits"]=>
    int(1)
    ["mime"]=>
    string(9) "image/png"
  }
}
