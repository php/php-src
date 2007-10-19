--TEST--
Test array_fill() function : basic functionality 
--FILE--
<?php
/* Prototype  : array array_fill(int $start_key, int $num, mixed $val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_fill() : basic functionality ***\n";

// calling the array_fill with all possible valid values for 'val' argument

$start_key = 0 ;
$num = 2;
$heredoc = <<<HERE_DOC
Hello
HERE_DOC;

// array of possible valid values for 'val' arugment
$values = array (
  
  /* 1  */  NULL,
            0,
            1,
  /* 4  */  1.5,
            'hi',
            "hi",
  /* 7  */  $heredoc

);

$counter = 1;
for($i = 0; $i < count($values); $i ++)
{
  echo "-- Iteration $counter --\n";
  $val = $values[$i];

  var_dump( array_fill($start_key, $num, $val) );

  $counter++;
}  

echo "Done";
?>
--EXPECTF--
*** Testing array_fill() : basic functionality ***
-- Iteration 1 --
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
-- Iteration 2 --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
-- Iteration 3 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
-- Iteration 4 --
array(2) {
  [0]=>
  float(1.5)
  [1]=>
  float(1.5)
}
-- Iteration 5 --
array(2) {
  [0]=>
  string(2) "hi"
  [1]=>
  string(2) "hi"
}
-- Iteration 6 --
array(2) {
  [0]=>
  string(2) "hi"
  [1]=>
  string(2) "hi"
}
-- Iteration 7 --
array(2) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(5) "Hello"
}
Done
