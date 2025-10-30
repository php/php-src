--TEST--
Test Uri\Rfc3986\Uri parsing - host - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://exḁmple.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
