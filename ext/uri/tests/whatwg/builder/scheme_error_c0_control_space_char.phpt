--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - error - contains leading and trailing C0 control and space characters
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setScheme(" \x01https \x02");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed (MissingSchemeNonRelativeUrl)
