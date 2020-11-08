--TEST--
Test extract() function (error conditions)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* Invalid second argument ( only 0-6 is valid) */
$arr = array(1);

try {
    var_dump( extract($arr, -1) );
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump( extract($arr, 7 , "wddr") );
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

/* Two Arguments, second as prefix but without prefix string as third argument */
try {
    var_dump( extract($arr,EXTR_PREFIX_IF_EXISTS) );
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing Error Conditions ***
extract(): Argument #2 ($flags) must be a valid extract type
extract(): Argument #2 ($flags) must be a valid extract type
extract(): Argument #3 ($prefix) is required when using this extract type
