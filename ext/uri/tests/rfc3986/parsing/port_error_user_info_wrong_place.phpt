--TEST--
Test Uri\Rfc3986\Uri parsing - port - user info component is in wrong place
--FILE--
<?php

try {
    var_dump(new Uri\Rfc3986\Uri("https://example.com:8080@username:password"));
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
