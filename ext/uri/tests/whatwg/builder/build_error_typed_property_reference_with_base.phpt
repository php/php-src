--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - soft errors assigned to typed property reference with base URL
--FILE--
<?php

class Errors
{
    public string $value = '';
}

$constructorErrors = new Errors();
$builderErrors = new Errors();

$base = new Uri\WhatWg\Url('https://example.com/base/path');

try {
    new Uri\WhatWg\Url('//127.0.0.1.', $base, $constructorErrors->value);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new Uri\WhatWg\UrlBuilder()
        ->setHost('127.0.0.1.')
        ->build($base, $builderErrors->value);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Errors::$value of type string
TypeError: Cannot assign array to reference held by property Errors::$value of type string
