--TEST--
Test Uri\WhatWg\UrlBuilder::setPassword() - error - file scheme
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("file");
$builder->setPath("C:/a.txt");
$builder->setPassword("password");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed
