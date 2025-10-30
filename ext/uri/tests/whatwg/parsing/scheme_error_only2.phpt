--TEST--
Test Uri\WhatWg\Url parsing - scheme - only scheme
--FILE--
<?php

try {
    new Uri\WhatWg\Url("http:/");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
The specified URI is malformed (HostMissing)
