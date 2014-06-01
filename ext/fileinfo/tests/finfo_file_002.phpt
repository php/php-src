--TEST--
finfo_file(): Testing mime types
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$fp = finfo_open(FILEINFO_MIME_TYPE);
$results = array();

foreach (glob(__DIR__ . "/resources/*") as $filename) {
	if (is_file($filename)) {
		$results["$filename"] = finfo_file($fp, $filename);
	}
}
ksort($results);

var_dump($results);
?>
--EXPECTF--
array(8) {
  ["%s/resources/dir.zip"]=>
  string(15) "application/zip"
  ["%s/resources/test.awk"]=>
  string(10) "text/plain"
  ["%s/resources/test.bmp"]=>
  string(14) "image/x-ms-bmp"
  ["%s/resources/test.gif"]=>
  string(9) "image/gif"
  ["%s/resources/test.jpg"]=>
  string(10) "image/jpeg"
  ["%s/resources/test.mp3"]=>
  string(10) "audio/mpeg"
  ["%s/resources/test.pdf"]=>
  string(15) "application/pdf"
  ["%s/resources/test.png"]=>
  string(9) "image/png"
}
