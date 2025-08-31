--TEST--
Test array_diff_uassoc() function : error conditions
--FILE--
<?php
echo "*** Testing array_diff_uassoc() : error conditions ***\n";

//Initialize array
$array1 = array("a" => "green", "b" => "brown", "c" => "blue", "red");
$array2 = array("a" => "green", "yellow", "red");
$array3 = array("a" => "green", "red");
$array4 = array();
$extra_arg = array(1, 2, 3, 4);

function key_compare_func($a, $b)
{
    if ($a === $b) {
        return 0;
    }
    return ($a > $b)? 1:-1;
}

//Test array_diff_uassoc with one more than the expected number of arguments
echo "\n-- Testing array_diff_uassoc() function with more than expected no. of arguments --\n";
try {
    var_dump( array_diff_uassoc($array1, $array2, "key_compare_func", $extra_arg) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( array_diff_uassoc($array1, $array2, $array3, $array4, "key_compare_func", $extra_arg) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Testing array_diff_uassoc with one less than the expected number of arguments
echo "\n-- Testing array_diff_uassoc() function with less than expected no. of arguments --\n";
try {
    var_dump( array_diff_uassoc($array1, $array2) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing array_diff_uassoc() : error conditions ***

-- Testing array_diff_uassoc() function with more than expected no. of arguments --
array_diff_uassoc(): Argument #4 must be a valid callback, array callback must have exactly two members
array_diff_uassoc(): Argument #6 must be a valid callback, array callback must have exactly two members

-- Testing array_diff_uassoc() function with less than expected no. of arguments --
array_diff_uassoc(): Argument #2 must be a valid callback, array callback must have exactly two members
