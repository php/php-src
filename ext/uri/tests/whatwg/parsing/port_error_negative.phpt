--TEST--
Test Uri\WhatWg\Url parsing - port - negative value
--FILE--
<?php

try {
    var_dump(new Uri\WhatWg\Url("http://example.com:-1"));
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (PortInvalid)
