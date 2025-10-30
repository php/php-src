--TEST--
Test Uri\Rfc3986\Uri parsing - path - reserved character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com/fo[o/ba]r/");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
