--TEST--
Test array_combine() function : usage variations - different arrays(Bug#43424)
--FILE--
<?php
/* Prototype  : array array_combine(array $keys, array $values)
 * Description: Creates an array by using the elements of the first parameter as keys
 *              and the elements of the second as the corresponding values
 * Source code: ext/standard/array.c
*/

/*
* Passing different types of arrays to both $keys and $values arguments and testing whether 
* array_combine() behaves in an expected way with the arguments passed to the function
*/

echo "*** Testing array_combine() : Passing different types of arrays to both \$keys and \$values argument ***\n";
/* Different heredoc strings passed as argument to arrays */
// heredoc with blank line
$blank_line = <<<EOT


EOT;

// heredoc with multiline string
$multiline_string = <<<EOT
hello world
The quick brown fox jumped over;
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

// arrays passed to $keys argument
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

       // array with repetative keys
/*19*/ array("One" => 1, "two" => 2, "One" => 10, "two" => 20, "three" => 3)
);

// loop through each sub-array within $arrays to check the behavior of array_combine()
// same arrays are passed to both $keys and $values
$iterator = 1;
foreach($arrays as $array) {
  echo "-- Iteration $iterator --\n";
  var_dump( array_combine($array, $array) );
  $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_combine() : Passing different types of arrays to both $keys and $values argument ***
-- Iteration 1 --
array(2) {
  [1]=>
  int(1)
  [2]=>
  int(2)
}
-- Iteration 2 --
array(2) {
  ["1.1"]=>
  float(1.1)
  ["2.2"]=>
  float(2.2)
}
-- Iteration 3 --
array(2) {
  [""]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iteration 4 --
array(0) {
}
-- Iteration 5 --
array(1) {
  [""]=>
  NULL
}
-- Iteration 6 --
array(6) {
  ["a"]=>
  string(3) "a"
  ["aaaa"]=>
  string(5) "aaaa"
  ["b"]=>
  string(1) "b"
  ["b	bbb"]=>
  string(5) "b	bbb"
  ["c"]=>
  string(1) "c"
  ["\[\]\!\@\#$\%\^\&\*\(\)\{\}"]=>
  string(27) "\[\]\!\@\#$\%\^\&\*\(\)\{\}"
}
-- Iteration 7 --
array(6) {
  ["a\v\f"]=>
  string(5) "a\v\f"
  ["aaaa\r"]=>
  string(6) "aaaa\r"
  ["b"]=>
  string(1) "b"
  ["b\tbbb"]=>
  string(6) "b\tbbb"
  ["c"]=>
  string(1) "c"
  ["\[\]\!\@\#\$\%\^\&\*\(\)\{\}"]=>
  string(28) "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"
}
-- Iteration 8 --
array(4) {
  ["
"]=>
  string(1) "
"
  ["hello world
The quick brown fox jumped over;
the lazy dog
This is a double quoted string"]=>
  string(88) "hello world
The quick brown fox jumped over;
the lazy dog
This is a double quoted string"
  ["hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"]=>
  string(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  ["11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"]=>
  string(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
}
-- Iteration 9 --
array(3) {
  ["one"]=>
  string(3) "one"
  ["two"]=>
  string(3) "two"
  ["three"]=>
  string(5) "three"
}
-- Iteration 10 --
array(3) {
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
-- Iteration 11 --
array(4) {
  [10]=>
  int(10)
  [20]=>
  int(20)
  [40]=>
  int(40)
  [30]=>
  int(30)
}
-- Iteration 12 --
array(3) {
  ["ten"]=>
  string(3) "ten"
  ["twenty"]=>
  string(6) "twenty"
  ["thirty"]=>
  string(6) "thirty"
}
-- Iteration 13 --
array(3) {
  [1]=>
  int(1)
  ["two"]=>
  string(3) "two"
  ["four"]=>
  string(4) "four"
}
-- Iteration 14 --
array(2) {
  ["null"]=>
  string(4) "null"
  [""]=>
  NULL
}
-- Iteration 15 --
array(4) {
  ["true"]=>
  string(4) "true"
  ["false"]=>
  string(5) "false"
  [""]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iteration 16 --
array(2) {
  ["emptys"]=>
  string(6) "emptys"
  [""]=>
  string(0) ""
}
-- Iteration 17 --
array(2) {
  [""]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iteration 18 --
array(3) {
  [4]=>
  int(4)
  [5]=>
  int(5)
  [6]=>
  int(6)
}
-- Iteration 19 --
array(3) {
  [10]=>
  int(10)
  [20]=>
  int(20)
  [3]=>
  int(3)
}
Done
