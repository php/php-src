--TEST--
Bug #80121: Null pointer deref if CurlHandle directly instantiated
--EXTENSIONS--
curl
--FILE--
<?php

try {
    new CurlHandle;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    new CurlMultiHandle;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    new CurlShareHandle;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot directly construct CurlHandle, use curl_init() instead
Error: Cannot directly construct CurlMultiHandle, use curl_multi_init() instead
Error: Cannot directly construct CurlShareHandle, use curl_share_init() instead
