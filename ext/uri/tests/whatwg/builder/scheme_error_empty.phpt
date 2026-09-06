--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - error - empty
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setScheme("");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed (MissingSchemeNonRelativeUrl)
