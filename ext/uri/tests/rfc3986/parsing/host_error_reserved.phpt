--TEST--
Test Uri\Rfc3986\Uri parsing - host - reserved character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://ex[a]mple.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
