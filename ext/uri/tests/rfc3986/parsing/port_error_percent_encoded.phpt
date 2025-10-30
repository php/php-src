--TEST--
Test Uri\Rfc3986\Uri parsing - port - percent encoded character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com:%30");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
