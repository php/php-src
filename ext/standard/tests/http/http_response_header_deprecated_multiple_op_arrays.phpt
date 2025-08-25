--TEST--
$http_reponse_header should warn once per oparray
--FILE--
<?php

function foo() {
    var_dump($http_response_header);
}

class C {
    public function bar() {
        var_dump($http_response_header);
    }
}

?>
--EXPECTF--
Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %s on line 4

Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %s on line 9
