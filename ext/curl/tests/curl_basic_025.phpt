--TEST--
Test curl_getinfo() function with CURLINFO_* from curl >= 7.72.0
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x074800) {
        exit("skip: test works only with curl >= 7.72.0");
}
?>
--FILE--
<?php

include 'server.inc';

$ch = curl_init();
$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_POSTFIELDS, "data");
curl_exec($ch);
var_dump(curl_getinfo($ch, CURLINFO_EFFECTIVE_METHOD));
curl_close($ch);
?>
--EXPECT--
string(4) "POST"
