--TEST--
enable_post_data_reading: always_populate_raw_post_data has no effect (1)
--INI--
enable_post_data_reading=0
always_populate_raw_post_data=1
--POST_RAW--
Content-Type: application/x-www-form-urlencoded
a=1&b=ZYX
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
var_dump($HTTP_RAW_POST_DATA);
var_dump(file_get_contents("php://input"));
var_dump(file_get_contents("php://input"));
?>
--EXPECTF--
array(0) {
}
array(0) {
}

Warning: Undefined variable $HTTP_RAW_POST_DATA in %s on line %d
NULL
string(9) "a=1&b=ZYX"
string(9) "a=1&b=ZYX"
