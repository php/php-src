--TEST--
Test UrlValidationErrorType singleton
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new \Uri\WhatWg\Url('http://localhost:99999');
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    var_dump($e->errors[0]->type === \Uri\WhatWg\UrlValidationErrorType::PortOutOfRange);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortOutOfRange)
bool(true)
