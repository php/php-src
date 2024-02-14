--TEST--
Test CurlHandle::getInfo() function with CURLINFO_* from curl >= 7.72.0
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
$ch = (new CurlHandle($url))->setOpt(CURLOPT_POSTFIELDS, "data");
$ch->exec();
var_dump($ch->getInfo(CURLINFO_EFFECTIVE_METHOD));
--EXPECT--
string(4) "POST"
