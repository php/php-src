--TEST--
Test printf() function : usage variations - unexpected values for format argument
--FILE--
<?php
/*
* Testing printf() : with different unexpected values for format argument other than the strings
*/

echo "*** Testing printf() : with unexpected values for format argument ***\n";

// initialing required variables
$arg1 = "second arg";
$arg2 = "third arg";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// declaring class
class sample
{
  public function __toString() {
    return "Object";
  }
}

// creating a file resource
$file_handle = fopen(__FILE__, 'r');

//array of values to iterate over
$values = array(

          // int data
/*1*/	  0,
          1,
          12345,
          -2345,

          // float data
/*5*/      10.5,
          -10.5,
          10.1234567e10,
          10.7654321E-10,
          .5,

          // array data
/*10*/    array(),
          array(0),
          array(1),
          array(1, 2),
          array('color' => 'red', 'item' => 'pen'),

          // null data
/*15*/    NULL,
          null,

          // boolean data
/*17*/    true,
          false,
          TRUE,
          FALSE,

          // empty data
/*21*/    "",
          '',

          // object data
/*23*/    new sample(),

          // undefined data
/*24*/    @$undefined_var,

          // unset data
/*25*/    @$unset_var,

          // resource data
/*26*/    $file_handle
);

// loop through each element of the array for format

$count = 1;
foreach($values as $value) {
  echo "\n-- Iteration $count --\n";

  // with default argument
  try {
    $result = printf($value);
    echo "\n";
    var_dump($result);
  } catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
  }

  // with two arguments
  try {
    $result = printf($value, $arg1);
    echo "\n";
    var_dump($result);
  } catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
  }

  // with three arguments
  try {
    $result = printf($value, $arg1, $arg2);
    echo "\n";
    var_dump($result);
  } catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
  }

  $count++;
};

// close the resource
fclose($file_handle);

?>
--EXPECT--
*** Testing printf() : with unexpected values for format argument ***

-- Iteration 1 --
0
int(1)
0
int(1)
0
int(1)

-- Iteration 2 --
1
int(1)
1
int(1)
1
int(1)

-- Iteration 3 --
12345
int(5)
12345
int(5)
12345
int(5)

-- Iteration 4 --
-2345
int(5)
-2345
int(5)
-2345
int(5)

-- Iteration 5 --
10.5
int(4)
10.5
int(4)
10.5
int(4)

-- Iteration 6 --
-10.5
int(5)
-10.5
int(5)
-10.5
int(5)

-- Iteration 7 --
101234567000
int(12)
101234567000
int(12)
101234567000
int(12)

-- Iteration 8 --
1.07654321E-9
int(13)
1.07654321E-9
int(13)
1.07654321E-9
int(13)

-- Iteration 9 --
0.5
int(3)
0.5
int(3)
0.5
int(3)

-- Iteration 10 --
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given

-- Iteration 11 --
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given

-- Iteration 12 --
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given

-- Iteration 13 --
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given

-- Iteration 14 --
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given
printf(): Argument #1 ($format) must be of type string, array given

-- Iteration 15 --

int(0)

int(0)

int(0)

-- Iteration 16 --

int(0)

int(0)

int(0)

-- Iteration 17 --
1
int(1)
1
int(1)
1
int(1)

-- Iteration 18 --

int(0)

int(0)

int(0)

-- Iteration 19 --
1
int(1)
1
int(1)
1
int(1)

-- Iteration 20 --

int(0)

int(0)

int(0)

-- Iteration 21 --

int(0)

int(0)

int(0)

-- Iteration 22 --

int(0)

int(0)

int(0)

-- Iteration 23 --
Object
int(6)
Object
int(6)
Object
int(6)

-- Iteration 24 --

int(0)

int(0)

int(0)

-- Iteration 25 --

int(0)

int(0)

int(0)

-- Iteration 26 --
printf(): Argument #1 ($format) must be of type string, resource given
printf(): Argument #1 ($format) must be of type string, resource given
printf(): Argument #1 ($format) must be of type string, resource given
