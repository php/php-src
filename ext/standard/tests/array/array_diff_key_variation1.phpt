--TEST--
array_diff_key() : type variations 
--FILE--
<?php
/*
* proto array array_diff_key(array arr1, array arr2 [, array ...])
* Function is implemented in ext/standard/array.c
*/
/*
* Testing how array_diff_key treats keys that are numbers, floating point numbers or strings.
*/
$arr1 = array(1 => 'a', 2 => 'b', 3 => 'c', 'key1' => 'value');
$arr2 = array(1.00 => 'a', 2.00 => 'b', 3.00 => 'c', 'key2' => 'value');
$arr3 = array('1' => 'a', '2' => 'b', '3' => 'c', 'key3' => 'value');
$arr4 = array('1.00' => 'a', '2.00' => 'b', '3.00' => 'c', 'key4' => 'value'); //$arr4 looks different to the other three arrays.
print "Result of comparing integers and floating point value:\n"; //1 and 1.00 are treated as the same thing
print_r(array_diff_key($arr1, $arr2));
print_r(array_diff_key($arr2, $arr1));
print "Result of comparing integers and strings containing an integers:\n"; //1 and the string 1 are treated as the same thing
print_r(array_diff_key($arr1, $arr3));
print_r(array_diff_key($arr3, $arr1));
print "Result of comparing integers and strings containing floating points:\n"; //1 and the string 1.00 are not treated as the same thing
print_r(array_diff_key($arr1, $arr4));
print_r(array_diff_key($arr4, $arr1));
print "Result of comparing floating points and strings containing integers:\n";
print_r(array_diff_key($arr2, $arr3)); //1.00 and the string 1 are treated as the same thing
print_r(array_diff_key($arr3, $arr2));
print "Result of comparing strings containing integers and strings containing floating points:\n"; //the strings 1 and 1.00 are not treated as the same thing.
print_r(array_diff_key($arr3, $arr4));
print_r(array_diff_key($arr4, $arr3));
?>
--EXPECTF--
Result of comparing integers and floating point value:
Array
(
    [key1] => value
)
Array
(
    [key2] => value
)
Result of comparing integers and strings containing an integers:
Array
(
    [key1] => value
)
Array
(
    [key3] => value
)
Result of comparing integers and strings containing floating points:
Array
(
    [1] => a
    [2] => b
    [3] => c
    [key1] => value
)
Array
(
    [1.00] => a
    [2.00] => b
    [3.00] => c
    [key4] => value
)
Result of comparing floating points and strings containing integers:
Array
(
    [key2] => value
)
Array
(
    [key3] => value
)
Result of comparing strings containing integers and strings containing floating points:
Array
(
    [1] => a
    [2] => b
    [3] => c
    [key3] => value
)
Array
(
    [1.00] => a
    [2.00] => b
    [3.00] => c
    [key4] => value
)
