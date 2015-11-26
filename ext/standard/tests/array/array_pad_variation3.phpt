--TEST--
Test array_pad() function : usage variations - possible values for 'pad_value' argument
--FILE--
<?php
/* Prototype  : array array_pad(array $input, int $pad_size, mixed $pad_value)
 * Description: Returns a copy of input array padded with pad_value to size pad_size 
 * Source code: ext/standard/array.c
*/

/* 
* Testing array_pad() function for expected behavior by passing
* different possible values for $pad_value argument.
* $input and $pad_size arguments take fixed value.
*/

echo "*** Testing array_pad() : possible values for \$pad_value argument ***\n";

// Initialise $input and $pad_size argument
$input = array(1, 2);
$pad_size = 4;

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

// get a reference variable
$value = "hello";
$reference = &$value;

// different values to be passed to $pad_value argument
$pad_values = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
      .5,

       // array data
/*10*/ array(),
       array(0),
       array(1),
       array(1, 2),
       array('color' => 'red', 'item' => 'pen'),

       // null data
/*15*/ NULL,
       null,

       // boolean data
/*17*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*21*/ "",
       '',

       // string data
/*23*/ "string",
       'string',
       $heredoc,

       // strings with different white spaces
/*26*/ "\v\fHello\t world!! \rstring\n",
       '\v\fHello\t world!! \rstring\n',

       // object data
/*28*/ new classA(),

       // undefined data
/*29*/ @$undefined_var,

       // unset data
/*30*/ @$unset_var,
  
       // resource variable
/*31*/ $fp,

       // reference variable
/*32*/ $reference
);

// loop through each element of $pad_values to check the behavior of array_pad()
$iterator = 1;
foreach($pad_values as $pad_value) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_pad($input, $pad_size, $pad_value) );  // positive 'pad_size'
  var_dump( array_pad($input, -$pad_size, $pad_value) );  // negative 'pad_size'
  $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing array_pad() : possible values for $pad_value argument ***
-- Iteration 1 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(0)
  [3]=>
  int(0)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 2 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  int(1)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 3 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(12345)
  [3]=>
  int(12345)
}
array(4) {
  [0]=>
  int(12345)
  [1]=>
  int(12345)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 4 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(-2345)
  [3]=>
  int(-2345)
}
array(4) {
  [0]=>
  int(-2345)
  [1]=>
  int(-2345)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 5 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(10.5)
  [3]=>
  float(10.5)
}
array(4) {
  [0]=>
  float(10.5)
  [1]=>
  float(10.5)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 6 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(-10.5)
  [3]=>
  float(-10.5)
}
array(4) {
  [0]=>
  float(-10.5)
  [1]=>
  float(-10.5)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 7 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(123456789000)
  [3]=>
  float(123456789000)
}
array(4) {
  [0]=>
  float(123456789000)
  [1]=>
  float(123456789000)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 8 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(1.23456789E-9)
  [3]=>
  float(1.23456789E-9)
}
array(4) {
  [0]=>
  float(1.23456789E-9)
  [1]=>
  float(1.23456789E-9)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 9 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  float(0.5)
  [3]=>
  float(0.5)
}
array(4) {
  [0]=>
  float(0.5)
  [1]=>
  float(0.5)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 10 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(0) {
  }
  [3]=>
  array(0) {
  }
}
array(4) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 11 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(1) {
    [0]=>
    int(0)
  }
  [3]=>
  array(1) {
    [0]=>
    int(0)
  }
}
array(4) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [0]=>
    int(0)
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 12 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(1) {
    [0]=>
    int(1)
  }
  [3]=>
  array(1) {
    [0]=>
    int(1)
  }
}
array(4) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 13 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [3]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
array(4) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 14 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
  [3]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
}
array(4) {
  [0]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
  [1]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 15 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  NULL
  [3]=>
  NULL
}
array(4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 16 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  NULL
  [3]=>
  NULL
}
array(4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 17 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 18 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 19 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}
array(4) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 20 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}
array(4) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 21 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
}
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 22 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
}
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 23 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(6) "string"
  [3]=>
  string(6) "string"
}
array(4) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 24 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(6) "string"
  [3]=>
  string(6) "string"
}
array(4) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "string"
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 25 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(11) "hello world"
  [3]=>
  string(11) "hello world"
}
array(4) {
  [0]=>
  string(11) "hello world"
  [1]=>
  string(11) "hello world"
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 26 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(25) "Hello	 world!! string
"
  [3]=>
  string(25) "Hello	 world!! string
"
}
array(4) {
  [0]=>
  string(25) "Hello	 world!! string
"
  [1]=>
  string(25) "Hello	 world!! string
"
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 27 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(30) "\v\fHello\t world!! \rstring\n"
  [3]=>
  string(30) "\v\fHello\t world!! \rstring\n"
}
array(4) {
  [0]=>
  string(30) "\v\fHello\t world!! \rstring\n"
  [1]=>
  string(30) "\v\fHello\t world!! \rstring\n"
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 28 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  object(classA)#%d (0) {
  }
  [3]=>
  object(classA)#%d (0) {
  }
}
array(4) {
  [0]=>
  object(classA)#%d (0) {
  }
  [1]=>
  object(classA)#%d (0) {
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 29 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  NULL
  [3]=>
  NULL
}
array(4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 30 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  NULL
  [3]=>
  NULL
}
array(4) {
  [0]=>
  NULL
  [1]=>
  NULL
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 31 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  resource(%d) of type (stream)
  [3]=>
  resource(%d) of type (stream)
}
array(4) {
  [0]=>
  resource(%d) of type (stream)
  [1]=>
  resource(%d) of type (stream)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
-- Iteration 32 --
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(5) "hello"
  [3]=>
  string(5) "hello"
}
array(4) {
  [0]=>
  string(5) "hello"
  [1]=>
  string(5) "hello"
  [2]=>
  int(1)
  [3]=>
  int(2)
}
Done
