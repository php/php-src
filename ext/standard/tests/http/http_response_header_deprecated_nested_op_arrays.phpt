--TEST--
$http_reponse_header should warn once per oparray even if nested
--FILE--
<?php

function foo() {
    $http_response_header = "foo";
    function nested() {
        echo $http_response_header;
    }
    echo $http_response_header;
}

?>
--EXPECTF--
Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %s on line 6
