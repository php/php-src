--TEST--
Test Uri\Rfc3986\UriBuilder::setUserInfo() - error - missing host with base URI
--FILE--
<?php

$base = new Uri\Rfc3986\Uri('https://example.com');

$builder = new Uri\Rfc3986\UriBuilder()
    ->setUserInfo('user:pass');

try {
    $builder->build($base);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: Cannot set a userinfo without having a host
