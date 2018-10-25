--TEST--
Test sizeof() function : usage variations - all kinds of unset variables for 'var' argument
--FILE--
<?php
/* Prototype  : int sizeof($mixed var[, int $mode])
 * Description: Counts an elements in an array. If Standard PHP library is installed,
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: count()
 */

echo "*** Testing sizeof() : usage variations ***\n";

echo "--- Testing sizeof() for all kinds of unset variables in default, Normal and Recursive Modes ---\n";

// class declaration
class test
{
  public $member1;
}

// get an resource variable
$fp = fopen(__FILE__, "r");

// array containing different types of variables
$values = array (
            // int values
  /* 1  */  0,
            1,
            // float values
  /* 3  */  10.5,
            -10.5,
            12.34e3,
  /* 6  */  12.34E-3,
            // string values
  /* 7  */  "string",
            'string',
            "",
  /* 10 */  '',
            // NULL values
  /* 11 */  NULL,
            null,
            // Boolean Values
  /* 12 */  TRUE,
            true,
            false,
  /* 16 */  FALSE,
            // array values
  /* 17 */  array(),
            array(1, 2, 3,4 , array(5, 6)),
            // object variable
  /* 19 */  new test(),
            // resource variable
  /* 20 */  $fp
);

// loop through the each element of the $values array for 'var' argument
// and check the functionality of sizeof()
$counter = 1;
foreach($values as $value)
{
  echo "-- Iteration $counter --\n";

  // unset the variable
  unset($value);

  // now check the size of unset variable when different modes are given
  echo "Default Mode: ";
  var_dump( sizeof($value) );
  echo "\n";

  echo "COUNT_NORMAL Mode: ";
  var_dump( sizeof($value, COUNT_NORMAL) );
  echo "\n";

  echo "COUNT_RECURSIVE Mode: ";
  var_dump( sizeof($value, COUNT_RECURSIVE) );
  echo "\n";

  $counter++;
}

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing sizeof() : usage variations ***
--- Testing sizeof() for all kinds of unset variables in default, Normal and Recursive Modes ---
-- Iteration 1 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 2 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 3 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 4 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 5 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 6 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 7 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 8 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 9 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 10 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 11 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 12 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 13 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 14 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 15 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 16 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 17 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 18 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 19 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

-- Iteration 20 --
Default Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_NORMAL Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

COUNT_RECURSIVE Mode: 
Notice: Undefined variable: value in %s on line %d

Warning: sizeof(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

Done
