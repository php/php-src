--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("http://usĕr:pąss@example.com");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
