--TEST--
Test Uri\Rfc3986\UriBuilder::build() - error - invalid internal query
--EXTENSIONS--
reflection
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
new ReflectionProperty($builder, 'query')->setValue($builder, '<');

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified query is malformed
