--TEST--
CurlUrl::set() with string containing null byte
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$invalidUrl = "http://www.example.com\0http://google.com";
$url = new CurlUrl();

try {
	$url->set(CURLUPART_URL, $invalidUrl);
} catch (CurlUrlException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
CurlUrl::set(): Argument #2 ($content) must not contain any null bytes
