--TEST--
curl_share_errno and curl_share_strerror basic test
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init();
$errno = curl_share_errno($sh);
echo $errno . PHP_EOL;
echo curl_share_strerror($errno) . PHP_EOL;

try {
    curl_share_setopt($sh, -1, -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$errno = curl_share_errno($sh);
echo $errno . PHP_EOL;
echo curl_share_strerror($errno) . PHP_EOL;
?>
--EXPECT--
0
No error
curl_share_setopt(): Argument #2 ($option) is not a valid cURL share option
1
Unknown share option
