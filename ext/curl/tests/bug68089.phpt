--TEST--
Bug #68089 (NULL byte injection - cURL lib)
--SKIPIF--
<?php
include 'skipif.inc';

?>
--FILE--
<?php
$url = "file:///etc/passwd\0http://google.com";
$ch = curl_init();

try {
    curl_setopt($ch, CURLOPT_URL, $url);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
Done
--EXPECT--
curl_setopt(): cURL option cannot contain any null-bytes
Done
