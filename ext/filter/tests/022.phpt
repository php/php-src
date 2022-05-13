--TEST--
filter_var() and FILTER_SANITIZE_EMAIL
--EXTENSIONS--
filter
--FILE--
<?php

var_dump(filter_var("a@b.c", FILTER_SANITIZE_EMAIL));
var_dump(filter_var("a[!@#$%^&*()@a@#$%^&*(.com@#$%^&*(", FILTER_SANITIZE_EMAIL));
var_dump(filter_var("white space here \ \ \" some more", FILTER_SANITIZE_EMAIL));
var_dump(filter_var("", FILTER_SANITIZE_EMAIL));
var_dump(filter_var("123456789000000", FILTER_SANITIZE_EMAIL));

echo "Done\n";
?>
--EXPECT--
string(5) "a@b.c"
string(30) "a[!@#$%^&*@a@#$%^&*.com@#$%^&*"
string(22) "whitespaceheresomemore"
string(0) ""
string(15) "123456789000000"
Done
