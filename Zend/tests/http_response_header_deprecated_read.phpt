--TEST--
Reading $http_response_header without explicit assignment triggers deprecation warning
--FILE--
<?php

function foo() {
    echo $http_response_header;
}

foo();

?>
--EXPECTF--
Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %s on line %d

Warning: Undefined variable $http_response_header in %s on line %d
