--TEST--
Test Uri\Rfc3986\Uri parsing - basic - base URI is a relative reference
--FILE--
<?php

try {
    var_dump(new Uri\Rfc3986\Uri('foo', new Uri\Rfc3986\Uri('bar')));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\InvalidUriException: The specified base URI must be absolute
