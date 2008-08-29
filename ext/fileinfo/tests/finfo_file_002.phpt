--TEST--
finfo_file(): Testing mime types
--FILE--
<?php

$fp = finfo_open(FILEINFO_MIME);
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
array(5) {
  [u"%s/resources/test.bmp"]=>
  string(14) "image/x-ms-bmp"
  [u"%s/resources/test.gif"]=>
  string(9) "image/gif"
  [u"%s/resources/test.jpg"]=>
  string(10) "image/jpeg"
  [u"%s/resources/test.pdf"]=>
  string(15) "application/pdf"
  [u"%s/resources/test.png"]=>
  string(9) "image/png"
}
