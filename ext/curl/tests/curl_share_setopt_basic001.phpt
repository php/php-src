--TEST--
curl_share_setopt basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

$sh = curl_share_init();
var_dump(curl_share_setopt($sh, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE));
var_dump(curl_share_setopt($sh, CURLSHOPT_UNSHARE, CURL_LOCK_DATA_DNS));

try {
    curl_share_setopt($sh, -1, 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
curl_share_setopt(): Argument #2 ($option) is not a valid cURL share option
