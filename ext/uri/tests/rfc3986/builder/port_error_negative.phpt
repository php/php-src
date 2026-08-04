--TEST--
Test Uri\Rfc3986\UriBuilder::setPort() - error - negative number
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setPort(-1);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified port is malformed
