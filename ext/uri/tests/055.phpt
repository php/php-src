--TEST--
Test InvalidUrlException constructor error handling
--EXTENSIONS--
uri
--FILE--
<?php

try {
    var_dump(new Uri\Rfc3986\Uri('foo', new Uri\Rfc3986\Uri('bar')));
} catch (Uri\InvalidUriException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Uri\InvalidUriException: The specified base URI must be absolute
