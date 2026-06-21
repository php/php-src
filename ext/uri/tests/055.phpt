--TEST--
Test InvalidUrlException constructor error handling
--FILE--
<?php

try {
    var_dump(new Uri\Rfc3986\Uri('foo', new Uri\Rfc3986\Uri('bar')));
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
The specified base URI must be absolute
