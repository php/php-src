--TEST--
Test Uri\Rfc3986\Uri parsing - scheme - multibyte characters
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("ƕŢŢƤƨ://example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
