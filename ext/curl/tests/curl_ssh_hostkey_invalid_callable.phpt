--TEST--
Test curl_setopt(_array)() with CURLOPT_SSH_HOSTKEYFUNCTION option
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x075400) {
    die("skip: blob options not supported for curl < 7.84.0");
}
?>
--FILE--
<?php

function testOption(CurlHandle $handle, int $option) {
    try {
        var_dump(curl_setopt($handle, $option, 'undefined'));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }

    try {
        var_dump(curl_setopt_array($handle, [$option => 'undefined']));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

$url = "https://example.com";
$ch = curl_init($url);
testOption($ch, CURLOPT_SSH_HOSTKEYFUNCTION);

?>
--EXPECT--
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_SSH_HOSTKEYFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_SSH_HOSTKEYFUNCTION, function "undefined" not found or invalid function name
