--TEST--
Test implode() function, problems with big numbers
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
var_dump( implode(" ", ["hello long", 999999999999999999, PHP_INT_MAX]));
var_dump( implode(" ", ["hello negative long", -999999999999999999, PHP_INT_MIN] ) );
var_dump( implode(" ", ["hello small long", -101, -100, -99, -90, -11, -10, -9, -1, 0, 1, 2, 9, 10, 11, 90, 99, 100, 101] ) );
echo "Done\n";
?>
--EXPECT--
string(49) "hello long 999999999999999999 9223372036854775807"
string(60) "hello negative long -999999999999999999 -9223372036854775808"
string(76) "hello small long -101 -100 -99 -90 -11 -10 -9 -1 0 1 2 9 10 11 90 99 100 101"
Done
