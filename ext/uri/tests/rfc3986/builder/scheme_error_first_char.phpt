--TEST--
Test Uri\Rfc3986\UriBuilder::setScheme() - error - first character is not alpha
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setScheme("1");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified scheme is malformed
