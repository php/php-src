--TEST--
Test sizeof() function : usage variations - for all scalar types and resource variable
--FILE--
<?php
/* Prototype  : int sizeof($mixed var[, int $mode])
 * Description: Counts an elements in an array. If Standard PHP library is installed,
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: count()
 */

echo "*** Testing sizeof() : usage variations ***\n";

echo "--- Testing sizeof() for all scalar types in default,COUNT_NORMAL and COUNT_RECURSIVE mode ---\n";
// get a resource variable
$fp = fopen(__FILE__, "r");

// array containing all scalar types
$values = array (
           // int values
  /* 1  */  0,
            1,

            // float values
  /* 3  */   10.5,
            -10.5,
            12.3456789000e10,
            12.3456789000E-10,
  /* 7  */  .5,

            // NULL values
  /* 8  */  NULL,
            null,

            // boolean values
  /* 10 */  TRUE,
            FALSE,
            true,
  /* 13 */  false,

            // string data
  /* 14 */  "",
            '',
            "string",
  /* 17 */  'string',

            // undefined variable
            @$undefined_var,

            // resource variable
  /* 19 */  $fp
);

// loop through the each value of the array for 'var' argument and check the behaviour of sizeof()
$counter = 1;
for($i = 0; $i < count($values); $i++)
{
    echo "-- Iteration $counter --\n";

    $value = $values[$i];

    echo "Default Mode: ";
    try {
        var_dump( sizeof($value) );
    } catch (\TypeError $e) {
        echo $e->getMessage() . "\n";
    }
    echo "\n";

    echo "COUNT_NORMAL Mode: ";
    try {
        var_dump( sizeof($value, COUNT_NORMAL) );
    } catch (\TypeError $e) {
        echo $e->getMessage() . "\n";
    }
    echo "\n";

    echo "COUNT_RECURSIVE Mode: ";
    try {
        var_dump( sizeof($value, COUNT_RECURSIVE) );
    } catch (\TypeError $e) {
        echo $e->getMessage() . "\n";
    }
    echo "\n";

    $counter++;
}

echo "Done";
?>
--EXPECT--
*** Testing sizeof() : usage variations ***
--- Testing sizeof() for all scalar types in default,COUNT_NORMAL and COUNT_RECURSIVE mode ---
-- Iteration 1 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 2 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 3 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 4 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 5 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 6 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 7 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 8 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 9 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 10 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 11 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 12 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 13 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 14 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 15 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 16 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 17 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 18 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

-- Iteration 19 --
Default Mode: Parameter must be an array or an object that implements Countable

COUNT_NORMAL Mode: Parameter must be an array or an object that implements Countable

COUNT_RECURSIVE Mode: Parameter must be an array or an object that implements Countable

Done