--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - error - first character is not alpha
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setScheme("1");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed (MissingSchemeNonRelativeUrl)
