--TEST--
Test Uri\Rfc3986\Uri parsing - path - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com/fȎȎ");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
