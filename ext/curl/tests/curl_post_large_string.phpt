--TEST--
CURL post data larger than 2GB (to test CURLOPT_POSTFIELDSIZE_LARGE)
--INI--
memory_limit=3G
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE < 8) die('skip 64-bit only');
include 'skipif-nocaddy.inc';
?>
--EXTENSIONS--
curl
--FILE--
<?php
$size = 2 ** 31 + 100; // a little bit more than a signed 32-bit int
$data = str_repeat('a', $size);

$ch = curl_init("https://localhost/show_upload_size");
curl_setopt_array($ch, [
    CURLOPT_RETURNTRANSFER => true,
    CURLOPT_POST => true,
    CURLOPT_POSTFIELDS => $data,
]);

$response = curl_exec($ch);
var_dump($response);

?>
--EXPECT--
string(28) "Content-length: =2147483748="
