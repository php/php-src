--TEST--
Test Uri\Rfc3986\UriBuilder::setScheme() - error - empty
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setScheme("");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified scheme is malformed
