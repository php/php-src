--TEST--
curl_url function with string containing null byte
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$invalidUrl = "http://www.example.com\0http://google.com";

try {
    curl_url($invalidUrl);
} catch (CurlUrlException $e) {
    echo $e->getMessage() . "\n";
}

try {
    new CurlUrl($invalidUrl);
} catch (CurlUrlException $e) {
    echo $e->getMessage() . "\n";
}

$url = new CurlUrl();
try {
    curl_url_set($url, CURLUPART_URL, $invalidUrl);
} catch (CurlUrlException $e) {
    echo $e->getMessage() . "\n";
}

try {
    $url->set(CURLUPART_URL, $invalidUrl);
} catch (CurlUrlException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
curl_url(): Argument #1 ($url) must not contain any null bytes
CurlUrl::__construct(): Argument #1 ($url) must not contain any null bytes
curl_url_set(): Argument #3 ($content) must not contain any null bytes
CurlUrl::set(): Argument #2 ($content) must not contain any null bytes
