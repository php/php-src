--TEST--
$http_reponse_header as a parameter name should not warn
--FILE--
<?php

function foo($http_response_header) {
    var_dump($http_response_header);
}

foo("OK");

?>
--EXPECT--
string(2) "OK"
