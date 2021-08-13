--TEST--
CURL file uploading
--EXTENSIONS--
curl
--FILE--
<?php
$file = new CURLFile(__DIR__ . '/curl_testdata1.txt');
var_dump(serialize($file));
?>
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'CURLFile' is not allowed in %s:%d
Stack trace:
#0 %s(%d): serialize(Object(CURLFile))
#1 {main}
  thrown in %s on line %d
