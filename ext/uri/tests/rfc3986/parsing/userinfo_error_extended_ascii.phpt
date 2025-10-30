--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - extended ASCII character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("http://úzör@example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
