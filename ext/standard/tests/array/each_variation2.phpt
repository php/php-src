--TEST--
Test each() function : usage variations - arrays of different data types
--FILE--
<?php
/* Prototype  : array each(array $arr)
 * Description: Return the currently pointed key..value pair in the passed array,
 * and advance the pointer to the next element
 * Source code: Zend/zend_builtin_functions.c
 */

/*
 * Pass arrays of different data types as $arr argument to each() to test behaviour
 */

echo "*** Testing each() : usage variations ***\n";

//get an unset variable
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

// arrays of different data types to be passed as $arr
$inputs = array(

       // int data
/*1*/  'int' => array(
	   0,
       1,
       12345,
       -2345,
       ),

       // float data
/*2*/  'float' => array(
       10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,
       ),

       // null data
/*3*/ 'null' => array(
       NULL,
       null,
       ),

       // boolean data
/*4*/ 'bool' => array(
       true,
       false,
       TRUE,
       FALSE,
       ),

       // empty data
/*5*/ 'empty string' => array(
       "",
       '',
       ),

/*6*/ 'empty array' => array(
       ),

       // string data
/*7*/ 'string' => array(
       "string",
       'string',
       $heredoc,
       ),

       // object data
/*8*/ 'object' => array(
       new classA(),
       ),

       // undefined data
/*9*/ 'undefined' => array(
       @$undefined_var,
       ),

       // unset data
/*10*/ 'unset' => array(
       @$unset_var,
       ),

       // resource variable
/*11*/ 'resource' => array(
       $fp
       ),
);

// loop through each element of $inputs to check the behavior of each()
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator: $key data --\n";
  var_dump( each($input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing each() : usage variations ***

-- Iteration 1: int data --

Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
array(4) {
  [1]=>
  int(0)
  ["value"]=>
  int(0)
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 2: float data --
array(4) {
  [1]=>
  float(10.5)
  ["value"]=>
  float(10.5)
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 3: null data --
array(4) {
  [1]=>
  NULL
  ["value"]=>
  NULL
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 4: bool data --
array(4) {
  [1]=>
  bool(true)
  ["value"]=>
  bool(true)
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 5: empty string data --
array(4) {
  [1]=>
  string(0) ""
  ["value"]=>
  string(0) ""
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 6: empty array data --
bool(false)

-- Iteration 7: string data --
array(4) {
  [1]=>
  string(6) "string"
  ["value"]=>
  string(6) "string"
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 8: object data --
array(4) {
  [1]=>
  object(classA)#%d (0) {
  }
  ["value"]=>
  object(classA)#%d (0) {
  }
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 9: undefined data --
array(4) {
  [1]=>
  NULL
  ["value"]=>
  NULL
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 10: unset data --
array(4) {
  [1]=>
  NULL
  ["value"]=>
  NULL
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}

-- Iteration 11: resource data --
array(4) {
  [1]=>
  resource(%d) of type (stream)
  ["value"]=>
  resource(%d) of type (stream)
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}
Done
