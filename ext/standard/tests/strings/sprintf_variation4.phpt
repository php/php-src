--TEST--
sprintf %u With signed integer 32bit
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php

/* example#5: various examples */
$n =  43951789;
$u = -43951789;

// notice the double %%, this prints a literal '%' character
var_dump(sprintf("%%u = '%u'", $n)); // unsigned integer representation of a positive integer
var_dump(sprintf("%%u = '%u'", $u)); // unsigned integer representation of a negative integer

?>
--EXPECT--
string(15) "%u = '43951789'"
string(17) "%u = '4251015507'"

