--TEST--
Test Uri\Rfc3986\UriBuilder::setQuery() - error - contains invalid special character
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setQuery("#foo");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified query is malformed
