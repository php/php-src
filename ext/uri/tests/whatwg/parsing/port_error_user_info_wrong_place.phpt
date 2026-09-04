--TEST--
Test Uri\WhatWg\Url parsing - port - user info component is in wrong place
--FILE--
<?php

try {
    var_dump(new Uri\WhatWg\Url("https://example.com:8080@username:password"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URI is malformed (PortInvalid)
