--TEST--
Test Uri\WhatWg\Url parsing - host - empty
--FILE--
<?php

try {
    new Uri\WhatWg\Url("///");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (MissingSchemeNonRelativeUrl)
