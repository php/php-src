--TEST--
Test CurlHandle::getInfo() function with CURLOPT_* from curl >= 7.85.0
--INI--
open_basedir=.
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075500) {
    exit("skip: test works only with curl >= 7.85.0");
}
?>
--FILE--
<?php
include 'server.inc';

$ch = curl_init();

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
$ch = new CurlHandle($url);
try {
  $ch->setOpt(CURLOPT_PROTOCOLS_STR, "FilE,DICT");
} catch (\CurlHandleException $ex) {
  echo 'Caught: ', $ex->getMessage(), PHP_EOL;
}

$ch->setOpt(CURLOPT_PROTOCOLS_STR, "DICT"))
   ->setOpt(CURLOPT_REDIR_PROTOCOLS_STR, "HTTP")
   ->exec();
var_dump('Done');
--EXPECTF--
Caught: The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
string(4) "Done"
