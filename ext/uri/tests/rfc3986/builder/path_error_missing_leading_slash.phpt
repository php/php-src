--TEST--
Test Uri\Rfc3986\UriBuilder::setPath() - error - missing a leading slash when the URI contains a host
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setPath("foo/bar/baz");
$builder->setHost("example.com");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: A path must be either empty or begin with "/" when the URI contains a host
