--TEST--
CurlUrl::get() with errors
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl();

try {
    $url->get(CURLUPART_SCHEME);
} catch (CurlUrlException $e) {
    var_dump($e->getCode() == CURLUE_NO_SCHEME);
}


?>
--EXPECT--
bool(true)
