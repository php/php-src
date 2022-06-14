--TEST--
curl_url_get() and CurlUrl::get() with errors
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = curl_url();
var_dump(curl_url_errno($url));
try {
    curl_url_get($url, CURLUPART_SCHEME);
} catch (CurlUrlException $e) {
    var_dump($e->getCode() == CURLUE_NO_SCHEME);
}
var_dump(curl_url_errno($url) == CURLUE_NO_SCHEME);

$url = new CurlUrl();
var_dump($url->getErrno());
try {
    $url->get(CURLUPART_SCHEME);
} catch (CurlUrlException $e) {
    var_dump($e->getCode() == CURLUE_NO_SCHEME);
}
var_dump($url->getErrno() == CURLUE_NO_SCHEME);
?>
--EXPECT--
int(0)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
