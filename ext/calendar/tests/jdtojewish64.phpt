--TEST--
Bug #64895: Integer overflow in SndToJewish
--SKIPIF--
<?php
include 'skipif.inc';
if (PHP_INT_SIZE == 4) {
        die("skip this test is for 64bit platform only");
}
?>
--FILE--
<?php
$a = array(38245310, 324542846, 324542847, 9223372036854743639);

foreach ($a as $x) var_dump(jdtojewish($x));
--EXPECT--
string(11) "2/22/103759"
string(12) "12/13/887605"
string(5) "0/0/0"
string(5) "0/0/0"
