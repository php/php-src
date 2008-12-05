--TEST--
Test bug #46739
--FILE--
<?php
$ch = curl_init('http://127.0.0.1:9/');

curl_exec($ch);
$info = curl_getinfo($ch);

echo (array_key_exists('content_type', $info)) ? "set" : "not set";
?>
--EXPECT--
set
