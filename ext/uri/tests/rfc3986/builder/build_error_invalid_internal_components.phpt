--TEST--
Test Uri\Rfc3986\UriBuilder::build() rejects invalid internal component values
--EXTENSIONS--
reflection
--FILE--
<?php

foreach ([
    'scheme' => ':',
    'host' => '[',
    'query' => '<',
    'fragment' => '<',
] as $property => $value) {

    $builder = new Uri\Rfc3986\UriBuilder();
    new ReflectionProperty($builder, $property)->setValue($builder, $value);

    try {
        $builder->build();
    } catch (Throwable $e) {
        echo $property, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
scheme: Uri\InvalidUriException: The specified scheme is malformed
host: Uri\InvalidUriException: The specified host is malformed
query: Uri\InvalidUriException: The specified query is malformed
fragment: Uri\InvalidUriException: The specified fragment is malformed
