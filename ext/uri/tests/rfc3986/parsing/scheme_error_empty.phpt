--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - empty
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("://example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
