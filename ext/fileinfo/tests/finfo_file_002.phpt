--TEST--
finfo_file(): Testing mime types
--EXTENSIONS--
fileinfo
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
array(%d) {
  ["%s/resources/dir.zip"]=>
  string(15) "application/zip"
  ["%s/resources/test.awk"]=>
  string(10) "text/plain"
  ["%s/resources/test.bmp"]=>
  string(9) "image/bmp"
  ["%s/resources/test.gif"]=>
  string(9) "image/gif"
  ["%s/resources/test.jpg"]=>
  string(10) "image/jpeg"
  ["%s/test.json"]=>
  string(16) "application/json"
  ["%s/resources/test.mp3"]=>
  string(10) "audio/mpeg"
  ["%s/resources/test.pdf"]=>
  string(15) "application/pdf"
  ["%s/resources/test.png"]=>
  string(9) "image/png"
  ["%s/resources/test.ppt"]=>
  string(29) "application/vnd.ms-powerpoint"
  ["%s/resources/test.tga"]=>
  string(11) "image/x-tga"
  ["%s/resources/test.webm"]=>
  string(10) "video/webm"
  ["%s/resources/test.woff"]=>
  string(9) "font/woff"
  ["%s/resources/test.xz"]=>
  string(16) "application/x-xz"
}
