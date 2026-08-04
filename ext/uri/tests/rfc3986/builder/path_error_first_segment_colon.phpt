--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - error - contains a colon in the first path segment when the URI doesn't contain a scheme
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setPath("fo:o/bar/baz");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The path must not begin with ":" when the URI does not contain a scheme
