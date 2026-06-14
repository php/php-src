--TEST--
Curl persistent share handles cannot be used with curl_share_setopt
--EXTENSIONS--
curl
--FILE--
<?php

$sh = curl_share_init_persistent([CURL_LOCK_DATA_DNS]);

try {
    curl_share_setopt($sh, CURLOPT_SHARE, CURL_LOCK_DATA_CONNECT);
} catch (\TypeError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
curl_share_setopt(): Argument #1 ($share_handle) must be of type CurlShareHandle, CurlSharePersistentHandle given
