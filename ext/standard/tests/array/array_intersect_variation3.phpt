--TEST--
Test array_intersect() function : usage variations - different arrays for 'arr1' argument
--FILE--
<?php
/*
* Passing different types of arrays to $arr1 argument and testing whether
* array_intersect() behaves in expected way with the other arguments passed to the function
* The $arr2 argument is a fixed array.
*/

echo "*** Testing array_intersect() : Passing different types of arrays to \$arr1 argument ***\n";

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

       // array with repetitive keys
/*19*/ array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
);


// array to be passsed to $arr2 argument
$arr2 = array (
  1, 1.1, "hello", "one", NULL, 2,
  'world', true, false, false => 5, 'aaaa\r', "aaaa\r",
  $numeric_string,  $diff_whitespaces,
  "one" => "ten", 4 => "four", "two" => 2, 2 => "two",
  '', null => "null", '' => 'emptys'
);

// loop through each sub-array within $arrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $arr1) {
  echo "-- Iterator $iterator --\n";

  // Calling array_intersect() with default arguments
  var_dump( array_intersect($arr1, $arr2) );

  // Calling array_intersect() with more arguments.
  // additional argument passed is the same as $arr1 argument
  var_dump( array_intersect($arr1, $arr2, $arr1) );
  $iterator++;
}

echo "Done";
?>
--EXPECT--
*** Testing array_intersect() : Passing different types of arrays to $arr1 argument ***
-- Iterator 1 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- Iterator 2 --
array(1) {
  [0]=>
  float(1.1)
}
array(1) {
  [0]=>
  float(1.1)
}
-- Iterator 3 --
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iterator 4 --
array(0) {
}
array(0) {
}
-- Iterator 5 --
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
-- Iterator 6 --
array(1) {
  [1]=>
  string(5) "aaaa"
}
array(1) {
  [1]=>
  string(5) "aaaa"
}
-- Iterator 7 --
array(1) {
  [1]=>
  string(6) "aaaa\r"
}
array(1) {
  [1]=>
  string(6) "aaaa\r"
}
-- Iterator 8 --
array(2) {
  [2]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  [3]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
}
array(2) {
  [2]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  [3]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
}
-- Iterator 9 --
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
-- Iterator 10 --
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
-- Iterator 11 --
array(0) {
}
array(0) {
}
-- Iterator 12 --
array(1) {
  ["one"]=>
  string(3) "ten"
}
array(1) {
  ["one"]=>
  string(3) "ten"
}
-- Iterator 13 --
array(3) {
  ["one"]=>
  int(1)
  [2]=>
  string(3) "two"
  [4]=>
  string(4) "four"
}
array(3) {
  ["one"]=>
  int(1)
  [2]=>
  string(3) "two"
  [4]=>
  string(4) "four"
}
-- Iterator 14 --
array(2) {
  ["NULL"]=>
  NULL
  ["null"]=>
  NULL
}
array(2) {
  ["NULL"]=>
  NULL
  ["null"]=>
  NULL
}
-- Iterator 15 --
array(2) {
  ["false"]=>
  bool(false)
  ["true"]=>
  bool(true)
}
array(2) {
  ["false"]=>
  bool(false)
  ["true"]=>
  bool(true)
}
-- Iterator 16 --
array(3) {
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
  ["emptys"]=>
  string(0) ""
}
array(3) {
  [""]=>
  string(6) "emptys"
  ["emptyd"]=>
  string(0) ""
  ["emptys"]=>
  string(0) ""
}
-- Iterator 17 --
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
-- Iterator 18 --
array(1) {
  [0]=>
  int(5)
}
array(1) {
  [0]=>
  int(5)
}
-- Iterator 19 --
array(0) {
}
array(0) {
}
Done
