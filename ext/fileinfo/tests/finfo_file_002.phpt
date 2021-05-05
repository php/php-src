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
  ["%s/resources/test.bmp"]=>
  string(14) "image/x-ms-bmp"
  ["%s/resources/test.gif"]=>
  string(9) "image/gif"
  ["%s/resources/test.jpg"]=>
  string(10) "image/jpeg"
  ["%s/resources/test.pdf"]=>
  string(15) "application/pdf"
  ["%s/resources/test.png"]=>
  string(9) "image/png"
}
