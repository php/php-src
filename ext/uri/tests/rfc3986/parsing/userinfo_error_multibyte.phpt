--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - multibyte character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("http://usĕr:pąss@example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
