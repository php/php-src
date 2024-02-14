--TEST--
Test CurlHandle::getInfo() function with CURLINFO_* and CURLOPT_* from curl >= 7.84.0
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075400) {
    exit("skip: test works only with curl >= 7.84.0");
}
?>
--FILE--
<?php
include 'server.inc';

$ch = new CurlHandle;
var_dump(array_key_exists('capath', $ch->getInfo()));
var_dump(array_key_exists('cainfo', $ch->getInfo()));

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
$ch->setOpt(CURLOPT_URL, $url)
   ->setOpt(CURLOPT_SSH_HOSTKEYFUNCTION, function ($ch, $keytype, $key, $keylen) {
    // Can't really trigger this in a test
    var_dump($keytype);
    var_dump($key);
    var_dump($keylen);
    return CURLKHMATCH_OK;
})->exec();

var_dump('Done');
--EXPECT--
bool(true)
bool(true)
string(4) "Done"
