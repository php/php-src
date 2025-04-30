--TEST--
Curl persistent share handles must be initialized with a non-empty $share_opts
--EXTENSIONS--
curl
--FILE--
<?php

try {
    $sh = curl_share_init_persistent([]);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
curl_share_init_persistent(): Argument #1 ($share_options) must not be empty
