--TEST--
Test UrlValidationErrorType singleton
--FILE--
<?php

try {
    new \Uri\WhatWg\Url('http://localhost:99999');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors[0]->type === \Uri\WhatWg\UrlValidationErrorType::PortOutOfRange);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortOutOfRange)
bool(true)
