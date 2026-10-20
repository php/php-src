--TEST--
$http_response_header dynamic fetch should warn
--FILE--
<?php

function http_response_header() {
    ${__FUNCTION__} = "OK";
    var_dump(${__FUNCTION__});
}

http_response_header();

?>
--EXPECTF--
Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %s on line %d
string(2) "OK"
