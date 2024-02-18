--TEST--
Test CurlHandle::setOpt() function with CURLOPT_* from curl >= 7.86.0
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

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
$ch = new CurlHandle($url);

// Return value depends on if support is enabled or not
$try = function(int $val) use ($ch) {
  try {
    $ch->setOpt(CURLOPT_WS_OPTIONS, $val);
    var_dump(true);
  } catch (\CurlHandleException $ex) {
    var_dump(false);
  }
};

$try(0);
$try(CURLWS_RAW_MODE);

$ch->exec();
--EXPECTF--
bool(%s)
bool(%s)
