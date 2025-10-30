--TEST--
Test Uri\WhatWg\Url parsing - host - empty
--FILE--
<?php

try {
    new Uri\WhatWg\Url("https://");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (HostMissing)
