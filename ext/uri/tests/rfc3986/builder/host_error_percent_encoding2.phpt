--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - error - invalid percent encoded octet in IPv6
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setHost("[2001:%308:85a3:0000:0000:8a2e:0370:7334]");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified host is malformed
