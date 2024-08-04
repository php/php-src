--TEST--
Test curl_setopt(_array)() with options that take callabes
--EXTENSIONS--
curl
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
testOption($ch, CURLOPT_PROGRESSFUNCTION);
testOption($ch, CURLOPT_XFERINFOFUNCTION);
testOption($ch, CURLOPT_FNMATCH_FUNCTION);
testOption($ch, CURLOPT_WRITEFUNCTION);
testOption($ch, CURLOPT_HEADERFUNCTION);
testOption($ch, CURLOPT_READFUNCTION);

?>
--EXPECT--
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_PROGRESSFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_PROGRESSFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_XFERINFOFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_XFERINFOFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_FNMATCH_FUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_FNMATCH_FUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_WRITEFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_WRITEFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_HEADERFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_HEADERFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_READFUNCTION, function "undefined" not found or invalid function name
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_READFUNCTION, function "undefined" not found or invalid function name
