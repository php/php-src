--TEST--
curl_reset(): CURLOPT_PRIVATE is cleared
--EXTENSIONS--
curl
--FILE--
<?php
$ch = curl_init();
curl_setopt($ch, CURLOPT_PRIVATE, "secret-marker");
var_dump(curl_getinfo($ch, CURLINFO_PRIVATE));
curl_reset($ch);
var_dump(curl_getinfo($ch, CURLINFO_PRIVATE));
?>
--EXPECT--
string(13) "secret-marker"
bool(false)
