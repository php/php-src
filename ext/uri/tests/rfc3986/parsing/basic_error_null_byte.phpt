--TEST--
Test Uri\Rfc3986\Uri parsing - basic - URI contains null byte
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://exam\0ple.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
