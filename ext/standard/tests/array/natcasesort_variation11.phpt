--TEST--
Test natcasesort() function : usage variations - Different array keys
--FILE--
<?php
/*
 * Pass arrays where the keys are different data types to test behaviour of natcasesort()
 */

echo "*** Testing natcasesort() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// arrays with keys as different data types to be passed as $array_arg
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
       12.3456789000e6 => 'large',
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

       // duplicate values
/*13*/ 'duplicate' => array(
       'foo' => 'bar',
       'baz' => 'bar',
       'hello' => 'world'
       ),

);

// loop through each element of $inputs to check the behavior of natcasesort()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump( natcasesort($input) );
    var_dump($input);
    $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing natcasesort() : usage variations ***

-- Iteration 1 --
bool(true)
array(4) {
  [-2345]=>
  string(8) "negative"
  [1]=>
  string(3) "one"
  [12345]=>
  string(8) "positive"
  [0]=>
  string(4) "zero"
}

-- Iteration 2 --
bool(true)
array(3) {
  [0]=>
  string(4) "half"
  [-10]=>
  string(8) "negative"
  [10]=>
  string(8) "positive"
}

-- Iteration 3 --
bool(true)
array(2) {
  [12345678]=>
  string(5) "large"
  [0]=>
  string(5) "small"
}

-- Iteration 4 --
bool(true)
array(1) {
  [""]=>
  string(6) "null 1"
}

-- Iteration 5 --
bool(true)
array(1) {
  [""]=>
  string(6) "null 2"
}

-- Iteration 6 --
bool(true)
array(2) {
  [0]=>
  string(6) "lowerf"
  [1]=>
  string(6) "lowert"
}

-- Iteration 7 --
bool(true)
array(2) {
  [0]=>
  string(6) "upperf"
  [1]=>
  string(6) "uppert"
}

-- Iteration 8 --
bool(true)
array(1) {
  [""]=>
  string(6) "emptyd"
}

-- Iteration 9 --
bool(true)
array(1) {
  [""]=>
  string(6) "emptys"
}

-- Iteration 10 --
bool(true)
array(3) {
  ["stringd"]=>
  string(7) "stringd"
  ["hello world"]=>
  string(7) "stringh"
  ["strings"]=>
  string(7) "strings"
}

-- Iteration 11 --
bool(true)
array(1) {
  [""]=>
  string(9) "undefined"
}

-- Iteration 12 --
bool(true)
array(1) {
  [""]=>
  string(5) "unset"
}

-- Iteration 13 --
bool(true)
array(3) {
  ["foo"]=>
  string(3) "bar"
  ["baz"]=>
  string(3) "bar"
  ["hello"]=>
  string(5) "world"
}
Done
