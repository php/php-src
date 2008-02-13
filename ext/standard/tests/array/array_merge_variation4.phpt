--TEST--
Test array_merge() function : usage variations - Diff. data types as array keys
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array with different data types as keys to test how array_merge 
 * adds it onto an existing array
 */

echo "*** Testing array_merge() : usage variations ***\n";

// Initialise function arguments not being substituted
$arr = array ('one' => 1, 'two' => 2);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// arrays with keys as different data types to be passed as $input
$inputs = array(

       // int data
/*1*/  'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative',
       ),

       // float data
/*2*/  'float' => array(
       10.5 => 'positive',
       -10.5 => 'negative',
       .5 => 'half',
       ),
       
/*3*/  'extreme floats' => array(
       12.3456789000e10 => 'large',
       12.3456789000E-10 => 'small',
       ),

       // null data
/*4*/  'null uppercase' => array(
       NULL => 'null 1',
       ), 
       
/*5*/  'null lowercase' => array(
       null => 'null 2',
       ),

       // boolean data
/*6*/ 'bool lowercase' => array(
       true => 'lowert',
       false => 'lowerf',
       ),
       
/*7*/  'bool uppercase' => array(
       TRUE => 'uppert',
       FALSE => 'upperf',
       ),
       
       // empty data
/*8*/ 'empty double quotes' => array(
       "" => 'emptyd',
       ),
       
/*9*/  'empty single quotes' => array(
       '' => 'emptys',
       ),

       // string data
/*10*/ 'string' => array(
       "stringd" => 'stringd',
       'strings' => 'strings',
       $heredoc => 'stringh',
       ),

       // undefined data
/*11*/ 'undefined' => array(
       @$undefined_var => 'undefined',
       ),

       // unset data
/*12*/ 'unset' => array(
       @$unset_var => 'unset',
       ),
);

// loop through each element of $inputs to check the behavior of array_merge
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator: $key data --\n";
  var_dump( array_merge($input, $arr) );
  var_dump( array_merge($arr, $input) );
  $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_merge() : usage variations ***

-- Iteration 1: int data --
array(6) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(8) "positive"
  [3]=>
  string(8) "negative"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(6) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(8) "positive"
  [3]=>
  string(8) "negative"
}

-- Iteration 2: float data --
array(5) {
  [0]=>
  string(8) "positive"
  [1]=>
  string(8) "negative"
  [2]=>
  string(4) "half"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(5) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(8) "positive"
  [1]=>
  string(8) "negative"
  [2]=>
  string(4) "half"
}

-- Iteration 3: extreme floats data --
array(4) {
  [0]=>
  string(5) "large"
  [1]=>
  string(5) "small"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(5) "large"
  [1]=>
  string(5) "small"
}

-- Iteration 4: null uppercase data --
array(3) {
  [""]=>
  string(6) "null 1"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(6) "null 1"
}

-- Iteration 5: null lowercase data --
array(3) {
  [""]=>
  string(6) "null 2"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(6) "null 2"
}

-- Iteration 6: bool lowercase data --
array(4) {
  [0]=>
  string(6) "lowert"
  [1]=>
  string(6) "lowerf"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(6) "lowert"
  [1]=>
  string(6) "lowerf"
}

-- Iteration 7: bool uppercase data --
array(4) {
  [0]=>
  string(6) "uppert"
  [1]=>
  string(6) "upperf"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [0]=>
  string(6) "uppert"
  [1]=>
  string(6) "upperf"
}

-- Iteration 8: empty double quotes data --
array(3) {
  [""]=>
  string(6) "emptyd"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(6) "emptyd"
}

-- Iteration 9: empty single quotes data --
array(3) {
  [""]=>
  string(6) "emptys"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(6) "emptys"
}

-- Iteration 10: string data --
array(5) {
  ["stringd"]=>
  string(7) "stringd"
  ["strings"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(5) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["stringd"]=>
  string(7) "stringd"
  ["strings"]=>
  string(7) "strings"
  ["hello world"]=>
  string(7) "stringh"
}

-- Iteration 11: undefined data --
array(3) {
  [""]=>
  string(9) "undefined"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(9) "undefined"
}

-- Iteration 12: unset data --
array(3) {
  [""]=>
  string(5) "unset"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  [""]=>
  string(5) "unset"
}
Done
--UEXPECTF--
*** Testing array_merge() : usage variations ***

-- Iteration 1: int data --
array(6) {
  [0]=>
  unicode(4) "zero"
  [1]=>
  unicode(3) "one"
  [2]=>
  unicode(8) "positive"
  [3]=>
  unicode(8) "negative"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(6) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  unicode(4) "zero"
  [1]=>
  unicode(3) "one"
  [2]=>
  unicode(8) "positive"
  [3]=>
  unicode(8) "negative"
}

-- Iteration 2: float data --
array(5) {
  [0]=>
  unicode(8) "positive"
  [1]=>
  unicode(8) "negative"
  [2]=>
  unicode(4) "half"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(5) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  unicode(8) "positive"
  [1]=>
  unicode(8) "negative"
  [2]=>
  unicode(4) "half"
}

-- Iteration 3: extreme floats data --
array(4) {
  [0]=>
  unicode(5) "large"
  [1]=>
  unicode(5) "small"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(4) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  unicode(5) "large"
  [1]=>
  unicode(5) "small"
}

-- Iteration 4: null uppercase data --
array(3) {
  [u""]=>
  unicode(6) "null 1"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u""]=>
  unicode(6) "null 1"
}

-- Iteration 5: null lowercase data --
array(3) {
  [u""]=>
  unicode(6) "null 2"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u""]=>
  unicode(6) "null 2"
}

-- Iteration 6: bool lowercase data --
array(4) {
  [0]=>
  unicode(6) "lowert"
  [1]=>
  unicode(6) "lowerf"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(4) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  unicode(6) "lowert"
  [1]=>
  unicode(6) "lowerf"
}

-- Iteration 7: bool uppercase data --
array(4) {
  [0]=>
  unicode(6) "uppert"
  [1]=>
  unicode(6) "upperf"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(4) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  unicode(6) "uppert"
  [1]=>
  unicode(6) "upperf"
}

-- Iteration 8: empty double quotes data --
array(3) {
  [u""]=>
  unicode(6) "emptyd"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u""]=>
  unicode(6) "emptyd"
}

-- Iteration 9: empty single quotes data --
array(3) {
  [u""]=>
  unicode(6) "emptys"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u""]=>
  unicode(6) "emptys"
}

-- Iteration 10: string data --
array(5) {
  [u"stringd"]=>
  unicode(7) "stringd"
  [u"strings"]=>
  unicode(7) "strings"
  [u"hello world"]=>
  unicode(7) "stringh"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(5) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u"stringd"]=>
  unicode(7) "stringd"
  [u"strings"]=>
  unicode(7) "strings"
  [u"hello world"]=>
  unicode(7) "stringh"
}

-- Iteration 11: undefined data --
array(3) {
  [u""]=>
  unicode(9) "undefined"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u""]=>
  unicode(9) "undefined"
}

-- Iteration 12: unset data --
array(3) {
  [u""]=>
  unicode(5) "unset"
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
}
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [u""]=>
  unicode(5) "unset"
}
Done