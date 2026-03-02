--TEST--
Test join() function : usage variations - sub array as argument
--FILE--
<?php
/*
 * test join() by passing pieces as array containing sub array(s)
*/

echo "*** Testing implode() : usage variations - sub arrays ***\n";
$sub_array = array(array(1,2,3,4), array(1 => "one", 2 => "two"), "PHP", 50);

// pieces as array containing sub array
var_dump(join("TEST", $sub_array));

// glue as array & pieces as array containing sub array
try {
    var_dump(join(array(1, 2, 3, 4), $sub_array));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

// numeric value as glue, pieces as array containing sub array
var_dump(join(2, $sub_array));

// using directly the sub_array as pieces
var_dump(join(", ", $sub_array[0]));
var_dump(join(", ", $sub_array[1]));

echo "Done\n";
?>
--EXPECTF--
*** Testing implode() : usage variations - sub arrays ***

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
string(27) "ArrayTESTArrayTESTPHPTEST50"
join(): Argument #1 ($separator) must be of type string, array given

Warning: Array to string conversion in %s on line %d

Warning: Array to string conversion in %s on line %d
string(18) "Array2Array2PHP250"
string(10) "1, 2, 3, 4"
string(8) "one, two"
Done
