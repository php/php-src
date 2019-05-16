--TEST--
Test extract() function (error conditions)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* Invalid second argument ( only 0-6 is valid) */
$arr = array(1);
var_dump( extract($arr, -1 . "wddr") );
var_dump( extract($arr, 7 , "wddr") );

/* Two Arguments, second as prefix but without prefix string as third argument */
var_dump( extract($arr,EXTR_PREFIX_IF_EXISTS) );

echo "Done\n";
?>
--EXPECTF--
*** Testing Error Conditions ***

Notice: A non well formed numeric value encountered in %s on line %d

Warning: extract(): Invalid extract type in %s on line %d
NULL

Warning: extract(): Invalid extract type in %s on line %d
NULL

Warning: extract(): specified extract type requires the prefix parameter in %s on line %d
NULL
Done
