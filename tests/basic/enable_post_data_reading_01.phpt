--TEST--
enable_post_data_reading: basic test
--INI--
enable_post_data_reading=0
--POST_RAW--
Content-Type: application/x-www-form-urlencoded
a=1&b=ZYX
--FILE--
<?php
var_dump($_FILES);
var_dump($_POST);
var_dump($HTTP_RAW_POST_DATA);
var_dump(file_get_contents("php://input"));
--EXPECTF--
array(0) {
}
array(0) {
}

Notice: Undefined variable: HTTP_RAW_POST_DATA in %s on line %d
NULL
string(9) "a=1&b=ZYX"
