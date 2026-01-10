--TEST--
Bug GH-9441 (fseek does not work with php://input when data is not preread)
--INI--
enable_post_data_reading=0
--POST_RAW--
Content-Type: application/unknown
a=123456789&b=ZYX
--FILE--
<?php
$input = fopen("php://input", "r");
var_dump(fseek($input, 10));
var_dump(ftell($input));
var_dump(fread($input, 10));
var_dump(file_get_contents("php://input"));
?>
--EXPECT--
int(0)
int(10)
string(7) "9&b=ZYX"
string(17) "a=123456789&b=ZYX"
