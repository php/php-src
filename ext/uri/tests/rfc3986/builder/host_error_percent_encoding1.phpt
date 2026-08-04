--TEST--
Test Uri\Rfc3986\UriBuilder::setHost() - error - invalid percent encoding octet in registered name
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setHost("ex%3mple.co");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified host is malformed
