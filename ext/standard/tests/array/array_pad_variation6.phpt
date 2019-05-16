--TEST--
Test array_pad() function : usage variations - different arrays for 'input' argument
--FILE--
<?php
/* Prototype  : array array_pad(array $input, int $pad_size, mixed $pad_value)
 * Description: Returns a copy of input array padded with pad_value to size pad_size
 * Source code: ext/standard/array.c
*/

/*
* Passing different arrays to $input argument and testing whether
* array_pad() behaves in an expected way with the other arguments passed to the function.
* The $pad_size and $pad_value arguments passed are fixed values.
*/

echo "*** Testing array_pad() : Passing different arrays to \$input argument ***\n";

/* Different heredoc strings */

// heredoc with blank line
$blank_line = <<<EOT


EOT;

// heredoc with multiline string
$multiline_string = <<<EOT
hello world
The big brown fox jumped over;
the lazy dog
This is a double quoted string
EOT;

// heredoc with different whitespaces
$diff_whitespaces = <<<EOT
hello\r world\t
1111\t\t != 2222\v\v
heredoc\ndouble quoted string. with\vdifferent\fwhite\vspaces
EOT;

// heredoc with quoted strings and numeric values
$numeric_string = <<<EOT
11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.\t 0000 = 0000\n
EOT;

// different arrays to be passed to $input argument
$inputs = array (
/*1*/  array(1, 2), // with default keys and numeric values
       array(1.1, 2.2), // with default keys & float values
       array(false,true), // with default keys and boolean values
       array(), // empty array
/*5*/  array(NULL), // with NULL
       array("a\v\f", "aaaa\r", "b\tbbb", "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"),  // with double quoted strings
       array('a\v\f', 'aaaa\r', 'b\tbbb', '\[\]\!\@\#\$\%\^\&\*\(\)\{\}'),  // with single quoted strings
       array("h1" => $blank_line, "h2" => $multiline_string, "h3" => $diff_whitespaces, $numeric_string),  // with heredocs

       // associative arrays
/*9*/  array(1 => "one", 2 => "two", 3 => "three"),  // explicit numeric keys, string values
       array("one" => 1, "two" => 2, "three" => 3 ),  // string keys & numeric values
       array( 1 => 10, 2 => 20, 4 => 40, 3 => 30),  // explicit numeric keys and numeric values
       array( "one" => "ten", "two" => "twenty", "three" => "thirty"),  // string key/value
       array("one" => 1, 2 => "two", 4 => "four"),  //mixed

       // associative array, containing null/empty/boolean values as key/value
/*14*/ array(NULL => "NULL", null => "null", "NULL" => NULL, "null" => null),
       array(true => "true", false => "false", "false" => false, "true" => true),
       array("" => "emptyd", '' => 'emptys', "emptyd" => "", 'emptys' => ''),
       array(1 => '', 2 => "", 3 => NULL, 4 => null, 5 => false, 6 => true),
       array('' => 1, "" => 2, NULL => 3, null => 4, false => 5, true => 6),

       // array with repetative keys
/*19*/ array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
);

// initialize the $pad_size and $pad_value arguments
$pad_size = 6;
$pad_value = "HELLO";

// loop through each sub-array within $inputs to check the behavior of array_pad()
$iterator = 1;
foreach($inputs as $input) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_pad($input, $pad_size, $pad_value) );  // positive 'pad_size'
  var_dump( array_pad($input, -$pad_size, $pad_value) );  // negative 'pad_size'
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_pad() : Passing different arrays to $input argument ***
-- Iteration 1 --
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  int(1)
  [5]=>
  int(2)
}
-- Iteration 2 --
array(6) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  float(1.1)
  [5]=>
  float(2.2)
}
-- Iteration 3 --
array(6) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  bool(false)
  [5]=>
  bool(true)
}
-- Iteration 4 --
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
-- Iteration 5 --
array(6) {
  [0]=>
  NULL
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  NULL
}
-- Iteration 6 --
array(6) {
  [0]=>
  string(3) "a"
  [1]=>
  string(5) "aaaa"
  [2]=>
  string(5) "b	bbb"
  [3]=>
  string(27) "\[\]\!\@\#$\%\^\&\*\(\)\{\}"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(3) "a"
  [3]=>
  string(5) "aaaa"
  [4]=>
  string(5) "b	bbb"
  [5]=>
  string(27) "\[\]\!\@\#$\%\^\&\*\(\)\{\}"
}
-- Iteration 7 --
array(6) {
  [0]=>
  string(5) "a\v\f"
  [1]=>
  string(6) "aaaa\r"
  [2]=>
  string(6) "b\tbbb"
  [3]=>
  string(28) "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "a\v\f"
  [3]=>
  string(6) "aaaa\r"
  [4]=>
  string(6) "b\tbbb"
  [5]=>
  string(28) "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"
}
-- Iteration 8 --
array(6) {
  ["h1"]=>
  string(1) "
"
  ["h2"]=>
  string(86) "hello world
The big brown fox jumped over;
the lazy dog
This is a double quoted string"
  ["h3"]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  [0]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  ["h1"]=>
  string(1) "
"
  ["h2"]=>
  string(86) "hello world
The big brown fox jumped over;
the lazy dog
This is a double quoted string"
  ["h3"]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  [2]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
}
-- Iteration 9 --
array(6) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(3) "one"
  [4]=>
  string(3) "two"
  [5]=>
  string(5) "three"
}
-- Iteration 10 --
array(6) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
}
-- Iteration 11 --
array(6) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(40)
  [3]=>
  int(30)
  [4]=>
  string(5) "HELLO"
  [5]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  int(10)
  [3]=>
  int(20)
  [4]=>
  int(40)
  [5]=>
  int(30)
}
-- Iteration 12 --
array(6) {
  ["one"]=>
  string(3) "ten"
  ["two"]=>
  string(6) "twenty"
  ["three"]=>
  string(6) "thirty"
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  ["one"]=>
  string(3) "ten"
  ["two"]=>
  string(6) "twenty"
  ["three"]=>
  string(6) "thirty"
}
-- Iteration 13 --
array(6) {
  ["one"]=>
  int(1)
  [0]=>
  string(3) "two"
  [1]=>
  string(4) "four"
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  ["one"]=>
  int(1)
  [3]=>
  string(3) "two"
  [4]=>
  string(4) "four"
}
-- Iteration 14 --
array(6) {
  [""]=>
  string(4) "null"
  ["NULL"]=>
  NULL
  ["null"]=>
  NULL
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [""]=>
  string(4) "null"
  ["NULL"]=>
  NULL
  ["null"]=>
  NULL
}
-- Iteration 15 --
array(6) {
  [0]=>
  string(4) "true"
  [1]=>
  string(5) "false"
  ["false"]=>
  bool(false)
  ["true"]=>
  bool(true)
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(4) "true"
  [3]=>
  string(5) "false"
  ["false"]=>
  bool(false)
  ["true"]=>
  bool(true)
}
-- Iteration 16 --
array(6) {
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
  ["emptys"]=>
  string(0) ""
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
  ["emptys"]=>
  string(0) ""
}
-- Iteration 17 --
array(6) {
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  bool(false)
  [6]=>
  bool(true)
}
array(6) {
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  NULL
  [4]=>
  NULL
  [5]=>
  bool(false)
  [6]=>
  bool(true)
}
-- Iteration 18 --
array(6) {
  [""]=>
  int(4)
  [0]=>
  int(5)
  [1]=>
  int(6)
  [2]=>
  string(5) "HELLO"
  [3]=>
  string(5) "HELLO"
  [4]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  [""]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
}
-- Iteration 19 --
array(6) {
  ["One"]=>
  int(10)
  ["two"]=>
  int(20)
  ["three"]=>
  int(3)
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
}
array(6) {
  [0]=>
  string(5) "HELLO"
  [1]=>
  string(5) "HELLO"
  [2]=>
  string(5) "HELLO"
  ["One"]=>
  int(10)
  ["two"]=>
  int(20)
  ["three"]=>
  int(3)
}
Done
