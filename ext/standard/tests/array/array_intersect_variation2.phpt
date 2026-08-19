--TEST--
Test array_intersect() function : usage variations - unexpected values for 'array2' argument
--FILE--
<?php
/*
* Testing array_intersect() function by passing values to $array2 argument other than arrays
* and see that function emits proper warning messages wherever expected.
* The $array1 argument is a fixed array.
*/

echo "*** Testing array_intersect() : Passing non-array values to \$array2 argument ***\n";

// array to be passsed to $array1 as default argument
$array1 = array(1, 2);

// arrays to be passed to optional argument
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

// unexpected values to be passed to $array2 argument
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

// loop through each sub-array within $arrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $unexpected_value) {
    echo "\n-- Iterator $iterator --";

    // Calling array_intersect() with default arguments
    try {
        var_dump( array_intersect($array1,$unexpected_value) );
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    // Calling array_intersect() with more arguments
    try {
        var_dump( array_intersect($array1, $unexpected_value, $arr3) );
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECT--
*** Testing array_intersect() : Passing non-array values to $array2 argument ***

-- Iterator 1 --TypeError: array_intersect(): Argument #2 must be of type array, int given
TypeError: array_intersect(): Argument #2 must be of type array, int given

-- Iterator 2 --TypeError: array_intersect(): Argument #2 must be of type array, int given
TypeError: array_intersect(): Argument #2 must be of type array, int given

-- Iterator 3 --TypeError: array_intersect(): Argument #2 must be of type array, int given
TypeError: array_intersect(): Argument #2 must be of type array, int given

-- Iterator 4 --TypeError: array_intersect(): Argument #2 must be of type array, int given
TypeError: array_intersect(): Argument #2 must be of type array, int given

-- Iterator 5 --TypeError: array_intersect(): Argument #2 must be of type array, float given
TypeError: array_intersect(): Argument #2 must be of type array, float given

-- Iterator 6 --TypeError: array_intersect(): Argument #2 must be of type array, float given
TypeError: array_intersect(): Argument #2 must be of type array, float given

-- Iterator 7 --TypeError: array_intersect(): Argument #2 must be of type array, float given
TypeError: array_intersect(): Argument #2 must be of type array, float given

-- Iterator 8 --TypeError: array_intersect(): Argument #2 must be of type array, float given
TypeError: array_intersect(): Argument #2 must be of type array, float given

-- Iterator 9 --TypeError: array_intersect(): Argument #2 must be of type array, float given
TypeError: array_intersect(): Argument #2 must be of type array, float given

-- Iterator 10 --TypeError: array_intersect(): Argument #2 must be of type array, null given
TypeError: array_intersect(): Argument #2 must be of type array, null given

-- Iterator 11 --TypeError: array_intersect(): Argument #2 must be of type array, null given
TypeError: array_intersect(): Argument #2 must be of type array, null given

-- Iterator 12 --TypeError: array_intersect(): Argument #2 must be of type array, true given
TypeError: array_intersect(): Argument #2 must be of type array, true given

-- Iterator 13 --TypeError: array_intersect(): Argument #2 must be of type array, false given
TypeError: array_intersect(): Argument #2 must be of type array, false given

-- Iterator 14 --TypeError: array_intersect(): Argument #2 must be of type array, true given
TypeError: array_intersect(): Argument #2 must be of type array, true given

-- Iterator 15 --TypeError: array_intersect(): Argument #2 must be of type array, false given
TypeError: array_intersect(): Argument #2 must be of type array, false given

-- Iterator 16 --TypeError: array_intersect(): Argument #2 must be of type array, string given
TypeError: array_intersect(): Argument #2 must be of type array, string given

-- Iterator 17 --TypeError: array_intersect(): Argument #2 must be of type array, string given
TypeError: array_intersect(): Argument #2 must be of type array, string given

-- Iterator 18 --TypeError: array_intersect(): Argument #2 must be of type array, string given
TypeError: array_intersect(): Argument #2 must be of type array, string given

-- Iterator 19 --TypeError: array_intersect(): Argument #2 must be of type array, string given
TypeError: array_intersect(): Argument #2 must be of type array, string given

-- Iterator 20 --TypeError: array_intersect(): Argument #2 must be of type array, string given
TypeError: array_intersect(): Argument #2 must be of type array, string given

-- Iterator 21 --TypeError: array_intersect(): Argument #2 must be of type array, classA given
TypeError: array_intersect(): Argument #2 must be of type array, classA given

-- Iterator 22 --TypeError: array_intersect(): Argument #2 must be of type array, null given
TypeError: array_intersect(): Argument #2 must be of type array, null given

-- Iterator 23 --TypeError: array_intersect(): Argument #2 must be of type array, null given
TypeError: array_intersect(): Argument #2 must be of type array, null given

-- Iterator 24 --TypeError: array_intersect(): Argument #2 must be of type array, resource given
TypeError: array_intersect(): Argument #2 must be of type array, resource given
Done
