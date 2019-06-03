--TEST--
Test array_intersect() function : usage variations - unexpected values for 'arr1' argument
--FILE--
<?php
/* Prototype  : array array_intersect(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments
 * Source code: ext/standard/array.c
*/

/*
* Testing array_intersect() function by passing values to $arr1 argument other than arrays
* and see that function emits proper warning messages wherever expected.
* The $arr2 argument is a fixed array.
*/

echo "*** Testing array_intersect() : Passing non-array values to \$arr1 argument ***\n";

// array to be passsed to $arr2 as default argument
$arr2 = array(1, 2);

// array to be passed to optional argument
$arr3 = array(1, 2, "one" => 1, "two" => 2);

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $arr1 argument
$arrays = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',

       // string data
/*18*/ "string",
       'string',
       $heredoc,

       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each sub-array within $arrrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $unexpected_value) {
    echo "\n-- Iterator $iterator --";

    // Calling array_intersect() with default arguments
    try {
        var_dump( array_intersect($unexpected_value,$arr2) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    // Calling array_intersect() with more arguments
    try {
        var_dump( array_intersect($unexpected_value, $arr2, $arr3) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECT--
*** Testing array_intersect() : Passing non-array values to $arr1 argument ***

-- Iterator 1 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iterator 2 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iterator 3 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iterator 4 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iterator 5 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iterator 6 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iterator 7 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iterator 8 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iterator 9 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iterator 10 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iterator 11 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iterator 12 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iterator 13 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iterator 14 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iterator 15 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iterator 16 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iterator 17 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iterator 18 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iterator 19 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iterator 20 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iterator 21 --Expected parameter 1 to be an array, object given
Expected parameter 1 to be an array, object given

-- Iterator 22 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iterator 23 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iterator 24 --Expected parameter 1 to be an array, resource given
Expected parameter 1 to be an array, resource given
Done
