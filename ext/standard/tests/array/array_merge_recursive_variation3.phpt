--TEST--
Test array_merge_recursive() function : usage variations - different arrays for 'arr1' argument
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
*/

/*
* Passing different arrays to $arr1 argument and testing whether
* array_merge_recursive() behaves in an expected way.
*/

echo "*** Testing array_merge_recursive() : Passing different arrays to \$arr1 argument ***\n";

/* Different heredoc strings */

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

// arrays passed to $arr1 argument
$arrays = array (
/*1*/  array(1, 2,), // with default keys and numeric values
       array(1.1, 2.2), // with default keys & float values
       array(false, true), // with default keys and boolean values
       array(), // empty array
/*5*/  array(NULL), // with NULL
       array("a\v\f", "aaaa\r", "b", "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"),  // with double quoted strings
       array('a\v\f', 'aaaa\r', 'b', '\[\]\!\@\#\$\%\^\&\*\(\)\{\}'),  // with single quoted strings
       array("h1" => $blank_line, "h2" => $multiline_string, "h3" => $diff_whitespaces),  // with heredocs

       // associative arrays
/*9*/  array(1 => "one", 2 => "two"),  // explicit numeric keys, string values
       array("one" => 1, "two" => 2, "1" => 1 ),  // string keys & numeric values
       array( 1 => 10, 2 => 20, 4 => 40),  // explicit numeric keys and numeric values
       array( "one" => "ten", "two" => "twenty"),  // string key/value
       array("one" => 1, 2 => "two", 4 => "four"),  //mixed

       // associative array, containing null/empty/boolean values as key/value
/*14*/ array(NULL => "NULL", null => "null", "NULL" => NULL, "null" => null),
       array(true => "true", false => "false", "false" => false, "true" => true),
       array("" => "emptyd", '' => 'emptys', "emptyd" => "", 'emptys' => ''),
       array(1 => '', 2 => "", 3 => NULL, 4 => null, 5 => false, 6 => true),
       array('' => 1, "" => 2, NULL => 3, null => 4, false => 5, true => 6),

       // array containing embedded arrays
/*15*/ array("str1", "array" => array("hello", 'world'), array(1, 2))
);

// initialise the second argument
$arr2 = array( 1 => "one", 2, "string" => "hello", "array" => array("a", "b", "c"));

// loop through each sub array of $arrays and check the behavior of array_merge_recursive()
$iterator = 1;
foreach($arrays as $arr1) {
  echo "-- Iteration $iterator --\n";

  // with default argument
  echo "-- With default argument --\n";
  var_dump( array_merge_recursive($arr1) );

  // with more arguments
  echo "-- With more arguments --\n";
  var_dump( array_merge_recursive($arr1, $arr2) );

  $iterator++;
}
  
echo "Done";
?>
--EXPECT--

*** Testing array_merge_recursive() : Passing different arrays to $arr1 argument ***
-- Iteration 1 --
-- With default argument --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
-- With more arguments --
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 2 --
-- With default argument --
array(2) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
}
-- With more arguments --
array(6) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 3 --
-- With default argument --
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
-- With more arguments --
array(6) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 4 --
-- With default argument --
array(0) {
}
-- With more arguments --
array(4) {
  [0]=>
  unicode(3) "one"
  [1]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 5 --
-- With default argument --
array(1) {
  [0]=>
  NULL
}
-- With more arguments --
array(5) {
  [0]=>
  NULL
  [1]=>
  unicode(3) "one"
  [2]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 6 --
-- With default argument --
array(4) {
  [0]=>
  unicode(3) "a"
  [1]=>
  unicode(5) "aaaa"
  [2]=>
  unicode(1) "b"
  [3]=>
  unicode(27) "\[\]\!\@\#$\%\^\&\*\(\)\{\}"
}
-- With more arguments --
array(8) {
  [0]=>
  unicode(3) "a"
  [1]=>
  unicode(5) "aaaa"
  [2]=>
  unicode(1) "b"
  [3]=>
  unicode(27) "\[\]\!\@\#$\%\^\&\*\(\)\{\}"
  [4]=>
  unicode(3) "one"
  [5]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 7 --
-- With default argument --
array(4) {
  [0]=>
  unicode(5) "a\v\f"
  [1]=>
  unicode(6) "aaaa\r"
  [2]=>
  unicode(1) "b"
  [3]=>
  unicode(28) "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"
}
-- With more arguments --
array(8) {
  [0]=>
  unicode(5) "a\v\f"
  [1]=>
  unicode(6) "aaaa\r"
  [2]=>
  unicode(1) "b"
  [3]=>
  unicode(28) "\[\]\!\@\#\$\%\^\&\*\(\)\{\}"
  [4]=>
  unicode(3) "one"
  [5]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 8 --
-- With default argument --
array(3) {
  [u"h1"]=>
  unicode(1) "
"
  [u"h2"]=>
  unicode(88) "hello world
The quick brown fox jumped over;
the lazy dog
This is a double quoted string"
  [u"h3"]=>
  unicode(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
}
-- With more arguments --
array(7) {
  [u"h1"]=>
  unicode(1) "
"
  [u"h2"]=>
  unicode(88) "hello world
The quick brown fox jumped over;
the lazy dog
This is a double quoted string"
  [u"h3"]=>
  unicode(88) "hello world	
1111		 != 2222
heredoc
double quoted string. withdifferentwhitespaces"
  [0]=>
  unicode(3) "one"
  [1]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 9 --
-- With default argument --
array(2) {
  [0]=>
  unicode(3) "one"
  [1]=>
  unicode(3) "two"
}
-- With more arguments --
array(6) {
  [0]=>
  unicode(3) "one"
  [1]=>
  unicode(3) "two"
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 10 --
-- With default argument --
array(3) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  int(1)
}
-- With more arguments --
array(7) {
  [u"one"]=>
  int(1)
  [u"two"]=>
  int(2)
  [0]=>
  int(1)
  [1]=>
  unicode(3) "one"
  [2]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 11 --
-- With default argument --
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(40)
}
-- With more arguments --
array(7) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(40)
  [3]=>
  unicode(3) "one"
  [4]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 12 --
-- With default argument --
array(2) {
  [u"one"]=>
  unicode(3) "ten"
  [u"two"]=>
  unicode(6) "twenty"
}
-- With more arguments --
array(6) {
  [u"one"]=>
  unicode(3) "ten"
  [u"two"]=>
  unicode(6) "twenty"
  [0]=>
  unicode(3) "one"
  [1]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 13 --
-- With default argument --
array(3) {
  [u"one"]=>
  int(1)
  [0]=>
  unicode(3) "two"
  [1]=>
  unicode(4) "four"
}
-- With more arguments --
array(7) {
  [u"one"]=>
  int(1)
  [0]=>
  unicode(3) "two"
  [1]=>
  unicode(4) "four"
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 14 --
-- With default argument --
array(3) {
  [u""]=>
  unicode(4) "null"
  [u"NULL"]=>
  NULL
  [u"null"]=>
  NULL
}
-- With more arguments --
array(7) {
  [u""]=>
  unicode(4) "null"
  [u"NULL"]=>
  NULL
  [u"null"]=>
  NULL
  [0]=>
  unicode(3) "one"
  [1]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 15 --
-- With default argument --
array(4) {
  [0]=>
  unicode(4) "true"
  [1]=>
  unicode(5) "false"
  [u"false"]=>
  bool(false)
  [u"true"]=>
  bool(true)
}
-- With more arguments --
array(8) {
  [0]=>
  unicode(4) "true"
  [1]=>
  unicode(5) "false"
  [u"false"]=>
  bool(false)
  [u"true"]=>
  bool(true)
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 16 --
-- With default argument --
array(3) {
  [u""]=>
  unicode(6) "emptys"
  [u"emptyd"]=>
  unicode(0) ""
  [u"emptys"]=>
  unicode(0) ""
}
-- With more arguments --
array(7) {
  [u""]=>
  unicode(6) "emptys"
  [u"emptyd"]=>
  unicode(0) ""
  [u"emptys"]=>
  unicode(0) ""
  [0]=>
  unicode(3) "one"
  [1]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 17 --
-- With default argument --
array(6) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(0) ""
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  bool(false)
  [5]=>
  bool(true)
}
-- With more arguments --
array(10) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(0) ""
  [2]=>
  NULL
  [3]=>
  NULL
  [4]=>
  bool(false)
  [5]=>
  bool(true)
  [6]=>
  unicode(3) "one"
  [7]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 18 --
-- With default argument --
array(3) {
  [u""]=>
  int(4)
  [0]=>
  int(5)
  [1]=>
  int(6)
}
-- With more arguments --
array(7) {
  [u""]=>
  int(4)
  [0]=>
  int(5)
  [1]=>
  int(6)
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
  [u"array"]=>
  array(3) {
    [0]=>
    unicode(1) "a"
    [1]=>
    unicode(1) "b"
    [2]=>
    unicode(1) "c"
  }
}
-- Iteration 19 --
-- With default argument --
array(3) {
  [0]=>
  unicode(4) "str1"
  [u"array"]=>
  array(2) {
    [0]=>
    unicode(5) "hello"
    [1]=>
    unicode(5) "world"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
-- With more arguments --
array(6) {
  [0]=>
  unicode(4) "str1"
  [u"array"]=>
  array(5) {
    [0]=>
    unicode(5) "hello"
    [1]=>
    unicode(5) "world"
    [2]=>
    unicode(1) "a"
    [3]=>
    unicode(1) "b"
    [4]=>
    unicode(1) "c"
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [2]=>
  unicode(3) "one"
  [3]=>
  int(2)
  [u"string"]=>
  unicode(5) "hello"
}
Done
