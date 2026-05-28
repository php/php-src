--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - too large number
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    /*
    TODO the username/password/port is sometimes silently disregarded when no validation happens:
     https://github.com/php/php-src/blob/27d7b799c0a13578ee0506b428b8ddc209ffb010/ext/lexbor/lexbor/url/url.c#L4382
     So maybe setPort() shouldn't validate either.
    */
    $builder->setPort(PHP_INT_MAX);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified port is malformed
