--TEST--
Test Uri\Rfc3986\UriBuilder::setUserInfo() - error - missing host
--FILE--
<?php

$builder = new Uri\Rfc3986\UriBuilder();
$builder->setUserInfo("user:pass");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The URI must contain a host if either the userinfo or the port component is present
