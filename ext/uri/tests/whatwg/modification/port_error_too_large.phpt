--TEST--
Test Uri\WhatWg\Url::withPort() - error - larger than a 16-bit unsigned integer
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("https://example.com");

try {
    $url->withPort(65536);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors[0]->type === Uri\WhatWg\UrlValidationErrorType::PortOutOfRange);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified port is malformed (PortOutOfRange)
bool(true)
