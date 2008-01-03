--TEST--
Test array_map() function : usage variations - failing built-in functions & language constructs
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Test array_map() by passing non-permmited built-in functions and language constructs i.e.
 *   echo(), array(), empty(), eval(), exit(), isset(), list(), print()
 */

echo "*** Testing array_map() : non-permmited built-in functions ***\n";

// array to be passed as arguments
$arr1 = array(1, 2);

// built-in functions & language constructs
$callback_names = array(
/*1*/  'echo',
       'array',
       'empty',
/*4*/  'eval',
       'exit',
       'isset',
       'list',
/*8*/  'print'
);
for($count = 0; $count < count($callback_names); $count++)
{
  echo "-- Iteration ".($count + 1)." --\n";
  var_dump( array_map($callback_names[$count], $arr1) );
}

echo "Done";
?>
--EXPECTF--
*** Testing array_map() : non-permmited built-in functions ***
-- Iteration 1 --

Warning: array_map(): The first argument, 'echo', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 2 --

Warning: array_map(): The first argument, 'array', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 3 --

Warning: array_map(): The first argument, 'empty', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 4 --

Warning: array_map(): The first argument, 'eval', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 5 --

Warning: array_map(): The first argument, 'exit', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 6 --

Warning: array_map(): The first argument, 'isset', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 7 --

Warning: array_map(): The first argument, 'list', should be either NULL or a valid callback in %s on line %d
NULL
-- Iteration 8 --

Warning: array_map(): The first argument, 'print', should be either NULL or a valid callback in %s on line %d
NULL
Done
