--TEST--
Test Uri\Rfc3986\UriBuilder::setPort() - error - missing host
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setPort(443);

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: Cannot set a port without having a host
