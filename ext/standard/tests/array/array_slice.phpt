--TEST--
Testing array_slice() function
--FILE--
<?php

$var_array = array(
                   array(),
                   array(1,2,3,4,5,6,7,8,9),
                   array("One", "Two", "Three", "Four", "Five"),
                   array(6, "six", 7, "seven", 8, "eight", 9, "nine"),
                   array( "a" => "aaa", "A" => "AAA", "c" => "ccc", "d" => "ddd", "e" => "eee"),
                   array("1" => "one", "2" => "two", "3" => "three", "4" => "four", "5" => "five"),
                   array(1 => "one", 2 => "two", 3 => 7, 4 => "four", 5 => "five"),
                   array("f" => "fff", "1" => "one", 4 => 6, "" => "blank", 2.4 => "float", "F" => "FFF",
                         "blank" => "", 3.7 => 3.7, 5.4 => 7, 6 => 8.6, '5' => "Five"),
                   array(12, "name", 'age', '45'),
                   array( array("oNe", "tWo", 4), array(10, 20, 30, 40, 50), array())
                 );

$num = 4;
$str = "john";

$counter = 1;
foreach ($var_array as $sub_array)
{
  /* variations with two arguments */
  /* offset values >, < and = 0    */

  echo"\n*** Iteration ".$counter." ***\n";
  echo"\n*** Variation with first two Arguments ***\n";
  var_dump ( array_slice($sub_array, 1) );
  var_dump ( array_slice($sub_array, 0) );
  var_dump ( array_slice($sub_array, -2) );

  /* variations with three arguments */
  /* offset value variations with length values  */
  echo"\n*** Variation with first three Arguments ***\n";
  var_dump ( array_slice($sub_array, 1, 3) );
  var_dump ( array_slice($sub_array, 1, 0) );
  var_dump ( array_slice($sub_array, 1, -3) );
  var_dump ( array_slice($sub_array, 0, 3) );
  var_dump ( array_slice($sub_array, 0, 0) );
  var_dump ( array_slice($sub_array, 0, -3) );
  var_dump ( array_slice($sub_array, -2, 3) );
  var_dump ( array_slice($sub_array, -2, 0 ) );
  var_dump ( array_slice($sub_array, -2, -3) );

  /* variations with four arguments */
  /* offset value, length value and preserve_key values variation */
  echo"\n*** Variation with first two arguments with preserve_key value TRUE ***\n";
  var_dump ( array_slice($sub_array, 1, 3, true) );
  var_dump ( array_slice($sub_array, 1, 0, true) );
  var_dump ( array_slice($sub_array, 1, -3, true) );
  var_dump ( array_slice($sub_array, 0, 3, true) );
  var_dump ( array_slice($sub_array, 0, 0, true) );
  var_dump ( array_slice($sub_array, 0, -3, true) );
  var_dump ( array_slice($sub_array, -2, 3, true) );
  var_dump ( array_slice($sub_array, -2, 0, true) );
  var_dump ( array_slice($sub_array, -2, -3, true) );
  $counter++;
}

  /* variation of offset and length to point to same element */
  echo"\n*** Typical Variation of offset and length  Arguments ***\n";
  var_dump (array_slice($var_array[2], 1, -3, true) );
  var_dump (array_slice($var_array[2], 1, -3, false) );
  var_dump (array_slice($var_array[2], -3, -2, true) );
  var_dump (array_slice($var_array[2], -3, -2, false) );

?>
--EXPECT--
*** Iteration 1 ***

*** Variation with first two Arguments ***
array(0) {
}
array(0) {
}
array(0) {
}

*** Variation with first three Arguments ***
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}

*** Iteration 2 ***

*** Variation with first two Arguments ***
array(8) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
  [5]=>
  int(7)
  [6]=>
  int(8)
  [7]=>
  int(9)
}
array(9) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
}
array(2) {
  [0]=>
  int(8)
  [1]=>
  int(9)
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
}
array(0) {
}
array(5) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(0) {
}
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}
array(2) {
  [0]=>
  int(8)
  [1]=>
  int(9)
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
array(0) {
}
array(5) {
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(0) {
}
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
}
array(2) {
  [7]=>
  int(8)
  [8]=>
  int(9)
}
array(0) {
}
array(0) {
}

*** Iteration 3 ***

*** Variation with first two Arguments ***
array(4) {
  [0]=>
  string(3) "Two"
  [1]=>
  string(5) "Three"
  [2]=>
  string(4) "Four"
  [3]=>
  string(4) "Five"
}
array(5) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
  [3]=>
  string(4) "Four"
  [4]=>
  string(4) "Five"
}
array(2) {
  [0]=>
  string(4) "Four"
  [1]=>
  string(4) "Five"
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  string(3) "Two"
  [1]=>
  string(5) "Three"
  [2]=>
  string(4) "Four"
}
array(0) {
}
array(1) {
  [0]=>
  string(3) "Two"
}
array(3) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
}
array(0) {
}
array(2) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
}
array(2) {
  [0]=>
  string(4) "Four"
  [1]=>
  string(4) "Five"
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
  [3]=>
  string(4) "Four"
}
array(0) {
}
array(1) {
  [1]=>
  string(3) "Two"
}
array(3) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
}
array(0) {
}
array(2) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
}
array(2) {
  [3]=>
  string(4) "Four"
  [4]=>
  string(4) "Five"
}
array(0) {
}
array(0) {
}

*** Iteration 4 ***

*** Variation with first two Arguments ***
array(7) {
  [0]=>
  string(3) "six"
  [1]=>
  int(7)
  [2]=>
  string(5) "seven"
  [3]=>
  int(8)
  [4]=>
  string(5) "eight"
  [5]=>
  int(9)
  [6]=>
  string(4) "nine"
}
array(8) {
  [0]=>
  int(6)
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
  [3]=>
  string(5) "seven"
  [4]=>
  int(8)
  [5]=>
  string(5) "eight"
  [6]=>
  int(9)
  [7]=>
  string(4) "nine"
}
array(2) {
  [0]=>
  int(9)
  [1]=>
  string(4) "nine"
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  string(3) "six"
  [1]=>
  int(7)
  [2]=>
  string(5) "seven"
}
array(0) {
}
array(4) {
  [0]=>
  string(3) "six"
  [1]=>
  int(7)
  [2]=>
  string(5) "seven"
  [3]=>
  int(8)
}
array(3) {
  [0]=>
  int(6)
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
}
array(0) {
}
array(5) {
  [0]=>
  int(6)
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
  [3]=>
  string(5) "seven"
  [4]=>
  int(8)
}
array(2) {
  [0]=>
  int(9)
  [1]=>
  string(4) "nine"
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
  [3]=>
  string(5) "seven"
}
array(0) {
}
array(4) {
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
  [3]=>
  string(5) "seven"
  [4]=>
  int(8)
}
array(3) {
  [0]=>
  int(6)
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
}
array(0) {
}
array(5) {
  [0]=>
  int(6)
  [1]=>
  string(3) "six"
  [2]=>
  int(7)
  [3]=>
  string(5) "seven"
  [4]=>
  int(8)
}
array(2) {
  [6]=>
  int(9)
  [7]=>
  string(4) "nine"
}
array(0) {
}
array(0) {
}

*** Iteration 5 ***

*** Variation with first two Arguments ***
array(4) {
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
  ["d"]=>
  string(3) "ddd"
  ["e"]=>
  string(3) "eee"
}
array(5) {
  ["a"]=>
  string(3) "aaa"
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
  ["d"]=>
  string(3) "ddd"
  ["e"]=>
  string(3) "eee"
}
array(2) {
  ["d"]=>
  string(3) "ddd"
  ["e"]=>
  string(3) "eee"
}

*** Variation with first three Arguments ***
array(3) {
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
  ["d"]=>
  string(3) "ddd"
}
array(0) {
}
array(1) {
  ["A"]=>
  string(3) "AAA"
}
array(3) {
  ["a"]=>
  string(3) "aaa"
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
}
array(0) {
}
array(2) {
  ["a"]=>
  string(3) "aaa"
  ["A"]=>
  string(3) "AAA"
}
array(2) {
  ["d"]=>
  string(3) "ddd"
  ["e"]=>
  string(3) "eee"
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
  ["d"]=>
  string(3) "ddd"
}
array(0) {
}
array(1) {
  ["A"]=>
  string(3) "AAA"
}
array(3) {
  ["a"]=>
  string(3) "aaa"
  ["A"]=>
  string(3) "AAA"
  ["c"]=>
  string(3) "ccc"
}
array(0) {
}
array(2) {
  ["a"]=>
  string(3) "aaa"
  ["A"]=>
  string(3) "AAA"
}
array(2) {
  ["d"]=>
  string(3) "ddd"
  ["e"]=>
  string(3) "eee"
}
array(0) {
}
array(0) {
}

*** Iteration 6 ***

*** Variation with first two Arguments ***
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "four"
  [3]=>
  string(4) "five"
}
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
array(2) {
  [0]=>
  string(4) "four"
  [1]=>
  string(4) "five"
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "four"
}
array(0) {
}
array(1) {
  [0]=>
  string(3) "two"
}
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
}
array(0) {
}
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  string(4) "four"
  [1]=>
  string(4) "five"
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
  [4]=>
  string(4) "four"
}
array(0) {
}
array(1) {
  [2]=>
  string(3) "two"
}
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
}
array(0) {
}
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
array(2) {
  [4]=>
  string(4) "four"
  [5]=>
  string(4) "five"
}
array(0) {
}
array(0) {
}

*** Iteration 7 ***

*** Variation with first two Arguments ***
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  int(7)
  [2]=>
  string(4) "four"
  [3]=>
  string(4) "five"
}
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  int(7)
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
array(2) {
  [0]=>
  string(4) "four"
  [1]=>
  string(4) "five"
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  string(3) "two"
  [1]=>
  int(7)
  [2]=>
  string(4) "four"
}
array(0) {
}
array(1) {
  [0]=>
  string(3) "two"
}
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  int(7)
}
array(0) {
}
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}
array(2) {
  [0]=>
  string(4) "four"
  [1]=>
  string(4) "five"
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [2]=>
  string(3) "two"
  [3]=>
  int(7)
  [4]=>
  string(4) "four"
}
array(0) {
}
array(1) {
  [2]=>
  string(3) "two"
}
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  int(7)
}
array(0) {
}
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
array(2) {
  [4]=>
  string(4) "four"
  [5]=>
  string(4) "five"
}
array(0) {
}
array(0) {
}

*** Iteration 8 ***

*** Variation with first two Arguments ***
array(9) {
  [0]=>
  string(3) "one"
  [1]=>
  int(6)
  [""]=>
  string(5) "blank"
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
  [3]=>
  float(3.7)
  [4]=>
  string(4) "Five"
  [5]=>
  float(8.6)
}
array(10) {
  ["f"]=>
  string(3) "fff"
  [0]=>
  string(3) "one"
  [1]=>
  int(6)
  [""]=>
  string(5) "blank"
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
  [3]=>
  float(3.7)
  [4]=>
  string(4) "Five"
  [5]=>
  float(8.6)
}
array(2) {
  [0]=>
  string(4) "Five"
  [1]=>
  float(8.6)
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  int(6)
  [""]=>
  string(5) "blank"
}
array(0) {
}
array(6) {
  [0]=>
  string(3) "one"
  [1]=>
  int(6)
  [""]=>
  string(5) "blank"
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
}
array(3) {
  ["f"]=>
  string(3) "fff"
  [0]=>
  string(3) "one"
  [1]=>
  int(6)
}
array(0) {
}
array(7) {
  ["f"]=>
  string(3) "fff"
  [0]=>
  string(3) "one"
  [1]=>
  int(6)
  [""]=>
  string(5) "blank"
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(4) "Five"
  [1]=>
  float(8.6)
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [1]=>
  string(3) "one"
  [4]=>
  int(6)
  [""]=>
  string(5) "blank"
}
array(0) {
}
array(6) {
  [1]=>
  string(3) "one"
  [4]=>
  int(6)
  [""]=>
  string(5) "blank"
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
}
array(3) {
  ["f"]=>
  string(3) "fff"
  [1]=>
  string(3) "one"
  [4]=>
  int(6)
}
array(0) {
}
array(7) {
  ["f"]=>
  string(3) "fff"
  [1]=>
  string(3) "one"
  [4]=>
  int(6)
  [""]=>
  string(5) "blank"
  [2]=>
  string(5) "float"
  ["F"]=>
  string(3) "FFF"
  ["blank"]=>
  string(0) ""
}
array(2) {
  [5]=>
  string(4) "Five"
  [6]=>
  float(8.6)
}
array(0) {
}
array(0) {
}

*** Iteration 9 ***

*** Variation with first two Arguments ***
array(3) {
  [0]=>
  string(4) "name"
  [1]=>
  string(3) "age"
  [2]=>
  string(2) "45"
}
array(4) {
  [0]=>
  int(12)
  [1]=>
  string(4) "name"
  [2]=>
  string(3) "age"
  [3]=>
  string(2) "45"
}
array(2) {
  [0]=>
  string(3) "age"
  [1]=>
  string(2) "45"
}

*** Variation with first three Arguments ***
array(3) {
  [0]=>
  string(4) "name"
  [1]=>
  string(3) "age"
  [2]=>
  string(2) "45"
}
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  int(12)
  [1]=>
  string(4) "name"
  [2]=>
  string(3) "age"
}
array(0) {
}
array(1) {
  [0]=>
  int(12)
}
array(2) {
  [0]=>
  string(3) "age"
  [1]=>
  string(2) "45"
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(3) {
  [1]=>
  string(4) "name"
  [2]=>
  string(3) "age"
  [3]=>
  string(2) "45"
}
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  int(12)
  [1]=>
  string(4) "name"
  [2]=>
  string(3) "age"
}
array(0) {
}
array(1) {
  [0]=>
  int(12)
}
array(2) {
  [2]=>
  string(3) "age"
  [3]=>
  string(2) "45"
}
array(0) {
}
array(0) {
}

*** Iteration 10 ***

*** Variation with first two Arguments ***
array(2) {
  [0]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [1]=>
  array(0) {
  }
}
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(3) "oNe"
    [1]=>
    string(3) "tWo"
    [2]=>
    int(4)
  }
  [1]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [2]=>
  array(0) {
  }
}
array(2) {
  [0]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [1]=>
  array(0) {
  }
}

*** Variation with first three Arguments ***
array(2) {
  [0]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [1]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(3) "oNe"
    [1]=>
    string(3) "tWo"
    [2]=>
    int(4)
  }
  [1]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [2]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}
array(2) {
  [0]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [1]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}

*** Variation with first two arguments with preserve_key value TRUE ***
array(2) {
  [1]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [2]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(3) "oNe"
    [1]=>
    string(3) "tWo"
    [2]=>
    int(4)
  }
  [1]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [2]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}
array(2) {
  [1]=>
  array(5) {
    [0]=>
    int(10)
    [1]=>
    int(20)
    [2]=>
    int(30)
    [3]=>
    int(40)
    [4]=>
    int(50)
  }
  [2]=>
  array(0) {
  }
}
array(0) {
}
array(0) {
}

*** Typical Variation of offset and length  Arguments ***
array(1) {
  [1]=>
  string(3) "Two"
}
array(1) {
  [0]=>
  string(3) "Two"
}
array(1) {
  [2]=>
  string(5) "Three"
}
array(1) {
  [0]=>
  string(5) "Three"
}
