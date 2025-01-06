--TEST--
Curl persistent share handle test with disallowed option
--EXTENSIONS--
curl
--FILE--
<?php

try {
    $sh = curl_share_init_persistent([CURL_LOCK_DATA_COOKIE]);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
curl_share_init_persistent(): Argument #1 ($share_options) must not contain CURL_LOCK_DATA_COOKIE because sharing cookies across PHP requests is unsafe
