--TEST--
Test curl_getinfo() function with CURLOPT_* from curl >= 7.86.0
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075600) {
    exit("skip: test works only with curl >= 7.86.0");
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
// Return value depends on if support is enabled or not
var_dump(curl_setopt($ch, CURLOPT_WS_OPTIONS, 0));
var_dump(curl_setopt($ch, CURLOPT_WS_OPTIONS, CURLWS_RAW_MODE));
curl_exec($ch);
curl_close($ch);
?>
--EXPECTF--
bool(%s)
bool(%s)
