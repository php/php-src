--TEST--
Test Uri\WhatWg\InvalidUrlException::__construct() - error - invalid errors element
--FILE--
<?php

try {
    new Uri\WhatWg\InvalidUrlException('message', [new stdClass()]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Uri\WhatWg\InvalidUrlException::__construct(): Argument #2 ($errors) must be a list of Uri\WhatWg\UrlValidationError
