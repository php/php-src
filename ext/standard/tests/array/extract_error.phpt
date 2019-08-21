--TEST--
Test extract() function (error conditions)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* Invalid second argument ( only 0-6 is valid) */
$arr = array(1);

try {
    var_dump( extract($arr, -1 . "wddr") );
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump( extract($arr, 7 , "wddr") );
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

/* Two Arguments, second as prefix but without prefix string as third argument */
try {
    var_dump( extract($arr,EXTR_PREFIX_IF_EXISTS) );
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--
*** Testing Error Conditions ***

Notice: A non well formed numeric value encountered in %s on line %d
Invalid extract type
Invalid extract type
Specified extract type requires the prefix parameter
Done
