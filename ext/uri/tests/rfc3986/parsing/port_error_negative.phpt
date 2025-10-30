--TEST--
Test Uri\Rfc3986\Uri parsing - port - negative value
--FILE--
<?php

try {
    var_dump(new Uri\Rfc3986\Uri("http://example.com:-1"));
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed
