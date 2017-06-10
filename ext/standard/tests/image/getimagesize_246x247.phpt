--TEST--
GetImageSize() with 246x247 pixels
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
		if (preg_match('/^246x247\./',$file)) {
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
  ["246x247.png"]=>
  array(6) {
    ["width"]=>
    int(246)
    ["height"]=>
    int(247)
    ["type"]=>
    int(3)
    ["text"]=>
    string(24) "width="246" height="247""
    ["bits"]=>
    int(4)
    ["mime"]=>
    string(9) "image/png"
  }
}
