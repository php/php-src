--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - contains a backslash delimiter
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setHost("example.com\\path");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostInvalidCodePoint)
