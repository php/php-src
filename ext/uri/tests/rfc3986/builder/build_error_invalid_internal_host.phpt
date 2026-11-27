--TEST--
Test Uri\Rfc3986\UriBuilder::build() - error - invalid internal host
--EXTENSIONS--
reflection
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
new ReflectionProperty($builder, 'host')->setValue($builder, '[');

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified host is malformed
