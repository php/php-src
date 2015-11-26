--TEST--
filter_var() and FILTER_SANITIZE_NUMBER_*
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("qwertyu123456dfghj", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("asd123123.asd123.23", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("123,23", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("0", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("asd123.2asd", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("qwertyuiop", FILTER_SANITIZE_NUMBER_INT));
var_dump(filter_var("123.4", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_var("123,4", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_var("123.4", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_THOUSAND));
var_dump(filter_var("123,4", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_THOUSAND));
var_dump(filter_var("123.4e", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_SCIENTIFIC));
var_dump(filter_var("123,4E", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_SCIENTIFIC));
var_dump(filter_var("qwe123,4qwe", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_var("werty65456.34", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_var("234.56fsfd", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_var("", FILTER_SANITIZE_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));

echo "Done\n";
?>
--EXPECT--	
string(6) "123456"
string(11) "12312312323"
string(5) "12323"
string(0) ""
string(1) "0"
string(4) "1232"
string(0) ""
string(5) "123.4"
string(4) "1234"
string(4) "1234"
string(5) "123,4"
string(5) "1234e"
string(5) "1234E"
string(4) "1234"
string(8) "65456.34"
string(6) "234.56"
string(0) ""
Done
