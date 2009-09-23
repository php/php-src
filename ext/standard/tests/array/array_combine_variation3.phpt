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

// heredoc with diferent whitespaces
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
  [u"1.1"]=>
  float(1.1)
  [u"2.2"]=>
  float(2.2)
}
-- Iteration 3 --
array(2) {
  [u""]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iteration 4 --

Warning: array_combine(): Both parameters should have at least 1 element in %s on line %d
bool(false)
-- Iteration 5 --
array(1) {
  [u""]=>
  NULL
}
-- Iteration 6 --
array(6) {
  [u"a"]=>
  unicode(3) "a"
  [u"aaaa"]=>
  unicode(5) "aaaa"
  [u"b"]=>
  unicode(1) "b"
  [u"b	bbb"]=>
  unicode(5) "b	bbb"
  [u"c"]=>
  unicode(1) "c"
  [u"\[\]\!\@\#$\%\^\&\*\(\)\{\}"]=>
  unicode(27) "\[\]\!\@\#$\%\^\&\*\(\)\{\}"
}
-- Iteration 7 --
array(6) {
  [u"a\v\f"]=>
  unicode(5) "a\v\f"
  [u"aaaa\r"]=>
  unicode(6) "aaaa\r"
  [u"b"]=>
  unicode(1) "b"
  [u"b\tbbb"]=>
  unicode(6) "b\tbbb"
  [u"c"]=>
  unicode(1) "c"
  [u"\[\]\!\@\#\$\%\^\&\*\(\)\{\}"]=>
  unicode(28) "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"
}
-- Iteration 8 --
array(4) {
  [u"
"]=>
  unicode(1) "
"
  [u"hello world
The quick brown fox jumped over;
the lazy dog
This is a double quoted string"]=>
  unicode(88) "hello world
The quick brown fox jumped over;
the lazy dog
This is a double quoted string"
  [u"hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"]=>
  unicode(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  [u"11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"]=>
  unicode(90) "11 < 12. 123 >22
'single quoted string'
"double quoted string"
2222 != 1111.	 0000 = 0000
"
}
-- Iteration 9 --
array(3) {
  [u"one"]=>
  unicode(3) "one"
  [u"two"]=>
  unicode(3) "two"
  [u"three"]=>
  unicode(5) "three"
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
  [u"ten"]=>
  unicode(3) "ten"
  [u"twenty"]=>
  unicode(6) "twenty"
  [u"thirty"]=>
  unicode(6) "thirty"
}
-- Iteration 13 --
array(3) {
  [1]=>
  int(1)
  [u"two"]=>
  unicode(3) "two"
  [u"four"]=>
  unicode(4) "four"
}
-- Iteration 14 --
array(2) {
  [u"null"]=>
  unicode(4) "null"
  [u""]=>
  NULL
}
-- Iteration 15 --
array(4) {
  [u"true"]=>
  unicode(4) "true"
  [u"false"]=>
  unicode(5) "false"
  [u""]=>
  bool(false)
  [1]=>
  bool(true)
}
-- Iteration 16 --
array(2) {
  [u"emptys"]=>
  unicode(6) "emptys"
  [u""]=>
  unicode(0) ""
}
-- Iteration 17 --
array(2) {
  [u""]=>
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
