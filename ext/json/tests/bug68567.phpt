--TEST--
Bug #68567 JSON_PARTIAL_OUTPUT_ON_ERROR can result in JSON with null key
--FILE--
<?php

var_dump(json_encode(array("\x80" => 1), JSON_PARTIAL_OUTPUT_ON_ERROR));

?>
--EXPECT--
string(6) "{"":1}"
