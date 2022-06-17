--TEST--
Create CurlUrl object with string containing null byte
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
    new CurlUrl($invalidUrl);
} catch (CurlUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
CurlUrl::__construct(): Argument #1 ($url) must not contain any null bytes
