--TEST--
Test curl_setopt(_array)() with options that take callabes
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

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

$url = "{$host}/get.inc";
$ch = curl_init($url);
testOption($ch, CURLOPT_PROGRESSFUNCTION);

?>
--EXPECT--
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_PROGRESSFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_PROGRESSFUNCTION, function "undefined" not found or invalid function name
