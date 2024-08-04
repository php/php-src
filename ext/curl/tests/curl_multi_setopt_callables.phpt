--TEST--
Test curl_multi_setopt() with options that take callabes
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();
try {
    var_dump(curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, 'undefined'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: curl_multi_setopt(): Argument #2 ($option) must be a valid callback for option CURLMOPT_PUSHFUNCTION, function "undefined" not found or invalid function name
