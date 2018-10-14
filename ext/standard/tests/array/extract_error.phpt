--TEST--
Test extract() function (error conditions)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* Zero Arguments */
var_dump( extract() );

/* Invalid second argument ( only 0-6 is valid) */
$arr = array(1);
var_dump( extract($arr, -1 . "wddr") );
var_dump( extract($arr, 7 , "wddr") );

/* scalar argument */
$val = 1;
var_dump( extract($val) );

/* string argument */
$str = "test";
var_dump( extract($str) );

/* More than valid number of arguments i.e. 3 args */
var_dump( extract($arr, EXTR_SKIP, "aa", "ee") );

/* Two Arguments, second as prefix but without prefix string as third argument */
var_dump( extract($arr,EXTR_PREFIX_IF_EXISTS) );

echo "Done\n";
?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: extract() expects at least 1 parameter, 0 given in %s on line %d
NULL

Notice: A non well formed numeric value encountered in %s on line %d

Warning: extract(): Invalid extract type in %s on line %d
NULL

Warning: extract(): Invalid extract type in %s on line %d
NULL

Warning: extract() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: extract() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: extract() expects at most 3 parameters, 4 given in %s on line %d
NULL

Warning: extract(): specified extract type requires the prefix parameter in %s on line %d
NULL
Done
