--TEST--
Integer overflow in SndToJewish leads to php hang 
--SKIPIF--
<?php 
include 'skipif.inc';
if (PHP_INT_SIZE == 4) {
        die("skip this test is for 64bit platform only");
}
?>
--FILE--
<?php
$a = array(38245310, 38245311, 9223372036854743639);

foreach ($a as $x) var_dump(jdtojewish($x));
--EXPECTF--
string(11) "2/22/103759"
string(5) "0/0/0"
string(5) "0/0/0"
