--TEST--
Test Uri\Rfc3986\Uri parsing - port - negative value
--FILE--
<?php

try {
    var_dump(new Uri\Rfc3986\Uri("http://example.com:-1"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Uri\InvalidUriException: The specified URI is malformed
