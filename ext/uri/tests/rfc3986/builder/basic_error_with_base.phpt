--TEST--
Test Uri\Rfc3986\UriBuilder basic - error - with base URL
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder()
    ->setPath("/foo/bar/baz");

try {
    $builder->build(new Uri\Rfc3986\Uri("/foo/bar"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified base URI must be absolute
