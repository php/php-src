--TEST--
Test array_intersect() function : usage variations - different arrays for 'arr2' argument
--FILE--
<?php
/* Prototype  : array array_intersect(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments
 * Source code: ext/standard/array.c
*/

/*
* Passing different types of arrays to $arr2 argument and testing whether
* array_intersect() behaves in expected way with the other arguments passed to the function.
* The $arr1 argument is a fixed array.
*/

echo "*** Testing array_intersect() : Passing different types of arrays to \$arr2 argument ***\n";

/* Different heredoc strings passed as argument to $arr2 */
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

// array to be passsed to $arr1 argument
$arr1 = array (
  1, 1.1, "hello", "one", NULL, 2,
  'world', true, false, false => 5, 'aaaa\r', "aaaa\r",
  $numeric_string, $diff_whitespaces,
  "one" => "ten", 4 => "four", "two" => 2, 2 => "two",
  '', null => "null", '' => 'emptys'
);

// arrays to be passed to $arr2 argument
$arrays = array (
/*1*/  array(1, 2), // array with default keys and numeric values
       array(1.1, 2.2), // array with default keys & float values
       array(false,true), // array with default keys and boolean values
       array(), // empty array
/*5*/  array(NULL), // array with NULL
       array("a\v\f","aaaa\r","b","b\tbbb","c","\[\]\!\@\#\$\%\^\&\*\(\)\{\}"),  // array with double quoted strings
       array('a\v\f','aaaa\r','b','b\tbbb','c','\[\]\!\@\#\$\%\^\&\*\(\)\{\}'),  // array with single quoted strings
       array($blank_line, $multiline_string, $diff_whitespaces, $numeric_string),  // array with heredocs

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

// loop through each sub-array within $arrrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $arr2) {
  echo "-- Iteration $iterator --\n";

  // Calling array_intersect() with default arguments
  var_dump( array_intersect($arr1, $arr2) );

  // Calling array_intersect() with more arguments
  // additional argument passed is the same as $arr1 argument
  var_dump( array_intersect($arr1, $arr2, $arr1) );
  $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect() : Passing different types of arrays to $arr2 argument ***
-- Iteration 1 --
array(3) {
  [5]=>
  int(2)
  [7]=>
  bool(true)
  ["two"]=>
  int(2)
}
array(3) {
  [5]=>
  int(2)
  [7]=>
  bool(true)
  ["two"]=>
  int(2)
}
-- Iteration 2 --
array(1) {
  [1]=>
  float(1.1)
}
array(1) {
  [1]=>
  float(1.1)
}
-- Iteration 3 --
array(3) {
  [7]=>
  bool(true)
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
array(3) {
  [7]=>
  bool(true)
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
-- Iteration 4 --
array(0) {
}
array(0) {
}
-- Iteration 5 --
array(2) {
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
array(2) {
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
-- Iteration 6 --
array(1) {
  [10]=>
  string(5) "aaaa"
}
array(1) {
  [10]=>
  string(5) "aaaa"
}
-- Iteration 7 --
array(1) {
  [9]=>
  string(6) "aaaa\r"
}
array(1) {
  [9]=>
  string(6) "aaaa\r"
}
-- Iteration 8 --
array(2) {
  [11]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
  [12]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
}
array(2) {
  [11]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
  [12]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
}
-- Iteration 9 --
array(2) {
  [2]=>
  string(3) "two"
  [3]=>
  string(3) "one"
}
array(2) {
  [2]=>
  string(3) "two"
  [3]=>
  string(3) "one"
}
-- Iteration 10 --
array(3) {
  [5]=>
  int(2)
  [7]=>
  bool(true)
  ["two"]=>
  int(2)
}
array(3) {
  [5]=>
  int(2)
  [7]=>
  bool(true)
  ["two"]=>
  int(2)
}
-- Iteration 11 --
array(0) {
}
array(0) {
}
-- Iteration 12 --
array(1) {
  ["one"]=>
  string(3) "ten"
}
array(1) {
  ["one"]=>
  string(3) "ten"
}
-- Iteration 13 --
array(3) {
  [2]=>
  string(3) "two"
  [4]=>
  string(4) "four"
  [7]=>
  bool(true)
}
array(3) {
  [2]=>
  string(3) "two"
  [4]=>
  string(4) "four"
  [7]=>
  bool(true)
}
-- Iteration 14 --
array(2) {
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
array(2) {
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
-- Iteration 15 --
array(3) {
  [7]=>
  bool(true)
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
array(3) {
  [7]=>
  bool(true)
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
-- Iteration 16 --
array(3) {
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
  [""]=>
  string(6) "emptys"
}
array(3) {
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
  [""]=>
  string(6) "emptys"
}
-- Iteration 17 --
array(3) {
  [7]=>
  bool(true)
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
array(3) {
  [7]=>
  bool(true)
  [8]=>
  bool(false)
  [13]=>
  string(0) ""
}
-- Iteration 18 --
array(1) {
  [0]=>
  int(5)
}
array(1) {
  [0]=>
  int(5)
}
-- Iteration 19 --
array(0) {
}
array(0) {
}
Done
