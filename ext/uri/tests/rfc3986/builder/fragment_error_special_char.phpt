--TEST--
Test Uri\Rfc3986\UriBuilder::setFragment() - error - contains invalid special character
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setFragment("#foo");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified fragment is malformed
