--TEST--
filter_data() and FS_EMAIL
--GET--

--FILE--
<?php

var_dump(filter_data("a@b.c", FS_EMAIL));
var_dump(filter_data("a[!@#$%^&*()@a@#$%^&*(.com@#$%^&*(", FS_EMAIL));
var_dump(filter_data("white space here \ \ \" som more", FS_EMAIL));
var_dump(filter_data("", FS_EMAIL));
var_dump(filter_data("123456789000000", FS_EMAIL));
	
echo "Done\n";
?>
--EXPECTF--	
string(5) "a@b.c"
string(30) "a[!@#$%^&*@a@#$%^&*.com@#$%^&*"
string(21) "whitespaceheresommore"
string(0) ""
string(15) "123456789000000"
Done
