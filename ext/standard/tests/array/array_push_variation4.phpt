--TEST--
Test array_push() function : usage variations - referenced variables
--INI--
allow_call_time_pass_reference=on
--FILE--
<?php
/* Prototype  : int array_push(array $stack, mixed $var [, mixed $...])
 * Description: Pushes elements onto the end of the array 
 * Source code: ext/standard/array.c
 */

/*
 * Test array_push when:
 * 1. passed referenced variables as $var arguments
 * 2. $var argument is a reference to $stack argument
 */

echo "*** Testing array_push() : usage variations ***\n";

$var1 = 'a';
$var2 = 'b';
$var3 = 'c';
$var4 = 'x';
$var5 = 'y';
$var6 = 'z';

$array = array(1, 2, 3);

echo "\n-- Pass array_push referenced varialbes as \$var arguments --\n";
var_dump(array_push($array, &$var1, &$var2, &$var3, &$var4, &$var5, &$var6));
var_dump($array);

echo "\n-- Pass \$var argument which is a reference to \$stack argument --\n";
var_dump(array_push($array, &$array));
var_dump($array);

/* break cycle */
$array[9] = null;

echo "Done";
?>
--EXPECTF--
*** Testing array_push() : usage variations ***

-- Pass array_push referenced varialbes as $var arguments --
int(9)
array(9) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  &string(1) "a"
  [4]=>
  &string(1) "b"
  [5]=>
  &string(1) "c"
  [6]=>
  &string(1) "x"
  [7]=>
  &string(1) "y"
  [8]=>
  &string(1) "z"
}

-- Pass $var argument which is a reference to $stack argument --
int(10)
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  &string(1) "a"
  [4]=>
  &string(1) "b"
  [5]=>
  &string(1) "c"
  [6]=>
  &string(1) "x"
  [7]=>
  &string(1) "y"
  [8]=>
  &string(1) "z"
  [9]=>
  &array(10) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    &string(1) "a"
    [4]=>
    &string(1) "b"
    [5]=>
    &string(1) "c"
    [6]=>
    &string(1) "x"
    [7]=>
    &string(1) "y"
    [8]=>
    &string(1) "z"
    [9]=>
    &array(10) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      &string(1) "a"
      [4]=>
      &string(1) "b"
      [5]=>
      &string(1) "c"
      [6]=>
      &string(1) "x"
      [7]=>
      &string(1) "y"
      [8]=>
      &string(1) "z"
      [9]=>
      *RECURSION*
    }
  }
}
Done
