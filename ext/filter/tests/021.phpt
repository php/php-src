--TEST--
filter_data() and FS_NUMBER_*
--GET--

--FILE--
<?php

var_dump(filter_data("qwertyu123456dfghj", FS_NUMBER_INT));
var_dump(filter_data("asd123123.asd123.23", FS_NUMBER_INT));
var_dump(filter_data("123,23", FS_NUMBER_INT));
var_dump(filter_data("", FS_NUMBER_INT));
var_dump(filter_data("0", FS_NUMBER_INT));
var_dump(filter_data("asd123.2asd", FS_NUMBER_INT));
var_dump(filter_data("qwertyuiop", FS_NUMBER_INT));
var_dump(filter_data("123.4", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_data("123,4", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_data("123.4", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_THOUSAND));
var_dump(filter_data("123,4", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_THOUSAND));
var_dump(filter_data("123.4e", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_SCIENTIFIC));
var_dump(filter_data("123,4E", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_SCIENTIFIC));
var_dump(filter_data("qwe123,4qwe", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_data("werty65456.34", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_data("234.56fsfd", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));
var_dump(filter_data("", FS_NUMBER_FLOAT, FILTER_FLAG_ALLOW_FRACTION));

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
