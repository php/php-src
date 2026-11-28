--TEST--
Test Uri\WhatWg\UrlBuilder::setHost() - error - contains a null byte
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setHost("\0bébé.be");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostInvalidCodePoint)
