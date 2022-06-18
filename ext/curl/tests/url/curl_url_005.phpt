--TEST--
CurlUrl::set() with errors
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
    $url->set('foobar://www.php.net');
} catch (CurlUrlException $e) {
    var_dump($e->getCode() == CurlUrlException::UNSUPPORTED_SCHEME);
}

?>
--EXPECT--
bool(true)
