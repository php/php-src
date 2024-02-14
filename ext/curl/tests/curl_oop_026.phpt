--TEST--
Test curl_getinfo() function with CURLOPT_* from curl >= 7.81.0
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075100) {
    exit("skip: test works only with curl >= 7.81.0");
}
?>
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$url = "{$host}/get.inc?test=";
(new CurlHandle($url))
  ->setOpt(CURLOPT_MIME_OPTIONS, CURLMIMEOPT_FORMESCAPE)
  ->exec();
var_dump("Done");
--EXPECT--
string(4) "Done"
