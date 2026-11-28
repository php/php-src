--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - contains a forbidden host code point
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setHost("exa mple.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostInvalidCodePoint)
