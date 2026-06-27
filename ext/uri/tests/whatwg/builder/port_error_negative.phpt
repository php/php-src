--TEST--
Test Uri\WhatWg\UrlBuilder::setPort() - error - negative number
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

try {
    $builder->setPort(-1);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified port is malformed
