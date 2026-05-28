--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - error - contains leading double slash when the host is not present
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setPath("//foo/bar/baz");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The path must not begin with "//" when the URI doesn't contain a host
