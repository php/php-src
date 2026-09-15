--TEST--
Test Uri\Rfc3986\UriBuilder::build() - error - invalid internal fragment
--EXTENSIONS--
reflection
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
new ReflectionProperty($builder, 'fragment')->setValue($builder, '<');

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified fragment is malformed
