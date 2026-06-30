--TEST--
Test Uri\Rfc3986\UriBuilder::setUserInfo() - error - invalid percent encoding
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();

try {
    $builder->setUserInfo("%3");
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified userinfo is malformed
