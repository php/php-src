--TEST--
Test Uri\Rfc3986\Uri parsing - userinfo - reserved character
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("http://us[er]:pass@example.com");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
