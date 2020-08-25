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
var_dump(curl_setopt($ch, CURLOPT_URL, $url));
?>
Done
--EXPECTF--
Warning: curl_setopt(): Curl option contains invalid characters (\0) in %s%ebug68089.php on line 4
bool(false)
Done
