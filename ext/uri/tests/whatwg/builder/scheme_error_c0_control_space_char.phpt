--TEST--
Test Uri\WhatWg\UrlBuilder::setScheme() - success - contains leading and trailing C0 control and space characters
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setScheme(" \x01https \x02");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified scheme is malformed
