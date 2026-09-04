--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - reserved character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("http://us[er]:pass@example.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
