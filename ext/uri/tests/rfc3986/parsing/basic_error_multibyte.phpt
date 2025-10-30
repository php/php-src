--TEST--
Test Uri\Rfc3986\Uri parsing - basic - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("🐘");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
