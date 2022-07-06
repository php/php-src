--TEST--
Test array_intersect_assoc() function : usage variations - different arrays for 'arr1' argument
--FILE--
<?php
/*
* Passing different types of arrays to $arr1 argument and testing whether
* array_intersect_assoc() behaves in an expected way with the other arguments passed to the function
* The $arr2 argument passed is a fixed array.
*/

echo "*** Testing array_intersect_assoc() : Passing different types of arrays to \$arr1 argument ***\n";

/* Different heredoc strings passed as argument to $arr1 */
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

// arrays to be passed to $arr1 argument
$arrays = array (
/*1*/  array(1, 2), // with default keys and numeric values
       array(1.1, 2.2), // with default keys & float values
       array(false,true), // with default keys and boolean values
       array(), // empty array
/*5*/  array(NULL), // with NULL
       array("a\v\f","aaaa\r","b","b\tbbb","c","\[\]\!\@\#\$\%\^\&\*\(\)\{\}"),  // with double quoted strings
       array('a\v\f','aaaa\r','b','b\tbbb','c','\[\]\!\@\#\$\%\^\&\*\(\)\{\}'),  // with single quoted strings
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

       // array with repetitive keys
/*19*/ array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
);


// array to be passsed to $arr2 argument
$arr2 = array (
  1, 1.1, 2.2, "hello", "one", NULL, 2,
  'world', true,5 => false, 1 => 'aaaa\r', "aaaa\r",
  'h3' => $diff_whitespaces, $numeric_string,
  "one" => "ten", 4 => "four", "two" => 2,
  '', null => "null", '' => 'emptys', "emptyd" => "",
);

// loop through each sub-array within $arrays to check the behavior of array_intersect_assoc()
$iterator = 1;
foreach($arrays as $arr1) {
  echo "-- Iteration $iterator --\n";

  // Calling array_intersect_assoc() with default arguments
  var_dump( array_intersect_assoc($arr1, $arr2) );

  // Calling array_intersect_assoc() with more arguments.
  // additional argument passed is the same as $arr1 argument
  var_dump( array_intersect_assoc($arr1, $arr2, $arr1) );
  $iterator++;
}

echo "Done";
?>
--EXPECT--
*** Testing array_intersect_assoc() : Passing different types of arrays to $arr1 argument ***
-- Iteration 1 --
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
-- Iteration 2 --
array(0) {
}
array(0) {
}
-- Iteration 3 --
array(0) {
}
array(0) {
}
-- Iteration 4 --
array(0) {
}
array(0) {
}
-- Iteration 5 --
array(0) {
}
array(0) {
}
-- Iteration 6 --
array(0) {
}
array(0) {
}
-- Iteration 7 --
array(1) {
  [1]=>
  string(6) "aaaa\r"
}
array(1) {
  [1]=>
  string(6) "aaaa\r"
}
-- Iteration 8 --
array(1) {
  ["h3"]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
}
array(1) {
  ["h3"]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
}
-- Iteration 9 --
array(0) {
}
array(0) {
}
-- Iteration 10 --
array(1) {
  ["two"]=>
  int(2)
}
array(1) {
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
array(1) {
  [4]=>
  string(4) "four"
}
array(1) {
  [4]=>
  string(4) "four"
}
-- Iteration 14 --
array(0) {
}
array(0) {
}
-- Iteration 15 --
array(0) {
}
array(0) {
}
-- Iteration 16 --
array(2) {
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
}
array(2) {
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
}
-- Iteration 17 --
array(1) {
  [5]=>
  bool(false)
}
array(1) {
  [5]=>
  bool(false)
}
-- Iteration 18 --
array(0) {
}
array(0) {
}
-- Iteration 19 --
array(0) {
}
array(0) {
}
Done
