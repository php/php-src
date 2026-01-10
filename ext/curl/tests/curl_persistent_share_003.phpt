--TEST--
Curl persistent share handle test with invalid option
--EXTENSIONS--
curl
--FILE--
<?php

try {
    $sh = curl_share_init_persistent([CURL_LOCK_DATA_DNS, CURL_LOCK_DATA_CONNECT, 30]);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
curl_share_init_persistent(): Argument #1 ($share_options) must contain only CURL_LOCK_DATA_* constants
