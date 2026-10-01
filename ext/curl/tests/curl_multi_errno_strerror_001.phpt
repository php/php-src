--TEST--
curl_multi_errno and curl_multi_strerror basic test
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();
$errno = curl_multi_errno($mh);
echo $errno . PHP_EOL;
echo curl_multi_strerror($errno) . PHP_EOL;

try {
    curl_multi_setopt($mh, -1, -1);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

$errno = curl_multi_errno($mh);
echo $errno . PHP_EOL;
echo curl_multi_strerror($errno) . PHP_EOL;
?>
--EXPECT--
0
No error
ValueError: curl_multi_setopt(): Argument #2 ($option) is not a valid cURL multi option
6
Unknown option
