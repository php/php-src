--TEST--
Test sizeof() function : usage variations - different array values for 'var' argument
--FILE--
<?php
/* Prototype  : int sizeof($mixed var[, int $mode])
 * Description: Counts an elements in an array. If Standard PHP library is installed, 
 * it will return the properties of an object.
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: count()
 */

echo "*** Testing sizeof() : usage variations ***\n";

// get a resource variable
$fp = fopen(__FILE__, "r");

echo "--- Testing sizeof() with different array values for 'var' argument ---\n";

// array containing different types of array values for 'var' argument 
$values = array (
  /* 1  */  array($fp, "resource" => $fp),
            array(1, array(3, 4, array(6, array(8)))),
            array("a" => 1, 'b' => 2, array( "c" =>3, array( "d" => 5))),
            array(),
  /* 5  */  array(1, 2, 3, 4),
            array("Saffron", "White", "Green"),
            array('saffron', 'white', 'green'),
            array(1 => "Hi", 2 => "Hello" ),
            array("color" => "red", "item" => "pen"),
  /* 10 */  array('color' => 'red', 'item' => 'pen'),
            array(TRUE => "red", FALSE => "pen" ),
            array(false => 'red', true => 'pen' ),
            array('color' => "red", "item" => 'pen', 1 => "Hi", "" => "Hello" ),
  /* 14 */  array($fp, "resource1" => $fp, 'resource2' => $fp, array( $fp, 'type' => $fp) )
);   

// loop through each element of the values array for 'var' argument 
// check the working of sizeof()
$counter = 1;
for($i = 0; $i < count($values); $i++)
{
  echo "-- Iteration $counter --\n";
  $var = $values[$i];

  echo "Default Mode: "; 
  var_dump( sizeof($var) );
  echo "\n";
  
  echo "COUNT_NORMAL Mode: ";
  var_dump( sizeof($var, COUNT_NORMAL) );
  echo "\n";

  echo "COUNT_RECURSIVE Mode: ";
  var_dump( sizeof($var, COUNT_RECURSIVE) );
  echo "\n";

  $counter++;
}
         
echo "Done";
?>
--EXPECTF--
*** Testing sizeof() : usage variations ***
--- Testing sizeof() with different array values for 'var' argument ---
-- Iteration 1 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(2)

-- Iteration 2 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(8)

-- Iteration 3 --
Default Mode: int(3)

COUNT_NORMAL Mode: int(3)

COUNT_RECURSIVE Mode: int(6)

-- Iteration 4 --
Default Mode: int(0)

COUNT_NORMAL Mode: int(0)

COUNT_RECURSIVE Mode: int(0)

-- Iteration 5 --
Default Mode: int(4)

COUNT_NORMAL Mode: int(4)

COUNT_RECURSIVE Mode: int(4)

-- Iteration 6 --
Default Mode: int(3)

COUNT_NORMAL Mode: int(3)

COUNT_RECURSIVE Mode: int(3)

-- Iteration 7 --
Default Mode: int(3)

COUNT_NORMAL Mode: int(3)

COUNT_RECURSIVE Mode: int(3)

-- Iteration 8 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(2)

-- Iteration 9 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(2)

-- Iteration 10 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(2)

-- Iteration 11 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(2)

-- Iteration 12 --
Default Mode: int(2)

COUNT_NORMAL Mode: int(2)

COUNT_RECURSIVE Mode: int(2)

-- Iteration 13 --
Default Mode: int(4)

COUNT_NORMAL Mode: int(4)

COUNT_RECURSIVE Mode: int(4)

-- Iteration 14 --
Default Mode: int(4)

COUNT_NORMAL Mode: int(4)

COUNT_RECURSIVE Mode: int(6)

Done
