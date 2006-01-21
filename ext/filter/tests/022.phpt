--TEST--
filter_data() and FILTER_SANITIZE_EMAIL
--FILE--
<?php

var_dump(filter_data("a@b.c", FILTER_SANITIZE_EMAIL));
var_dump(filter_data("a[!@#$%^&*()@a@#$%^&*(.com@#$%^&*(", FILTER_SANITIZE_EMAIL));
var_dump(filter_data("white space here \ \ \" som more", FILTER_SANITIZE_EMAIL));
var_dump(filter_data("", FILTER_SANITIZE_EMAIL));
var_dump(filter_data("123456789000000", FILTER_SANITIZE_EMAIL));
	
echo "Done\n";
?>
--EXPECTF--	
string(5) "a@b.c"
string(30) "a[!@#$%^&*@a@#$%^&*.com@#$%^&*"
string(21) "whitespaceheresommore"
string(0) ""
string(15) "123456789000000"
Done
