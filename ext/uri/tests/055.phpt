--TEST--
Test InvalidUrlException constructor error handling
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
