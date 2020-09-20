--TEST--
Test curl_getinfo() function with CURLINFO_HTTP_VERSION parameter
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip";
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x073200) {
        exit("skip: test works only with curl >= 7.50.0");
}
?>
--FILE--
<?php

include 'server.inc';

$ch = curl_init();
var_dump(0 === curl_getinfo($ch, CURLINFO_HTTP_VERSION));

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_exec($ch);
var_dump(CURL_HTTP_VERSION_1_1 === curl_getinfo($ch, CURLINFO_HTTP_VERSION));
curl_close($ch);
?>
--EXPECT--
bool(true)
bool(true)
