--TEST--
curl_url() function with string containing null byte
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = "http://www.example.com\0http://google.com";
try {
    curl_url($url);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
curl_url(): string must not contain any null bytes
