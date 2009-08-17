--TEST--
Test str_replace() function - test search values
--INI--
precision=14
--FILE--
<?php
/* 
  Prototype: mixed str_replace(mixed $search, mixed $replace, 
                               mixed $subject [, int &$count]);
  Description: Replace all occurrences of the search string with 
               the replacement string
*/


echo "\n*** Testing str_replace() with various search values ***";
$search_arr = array( TRUE, FALSE, 1, 0, -1, "1", "0", "-1",  NULL, 
                     array(), "php", "");

$i = 0;
/* loop through to replace the matched elements in the array */
foreach( $search_arr as $value ) {
  echo "\n-- Iteration $i --\n";
  /* replace the string in array */
  var_dump( str_replace($value, "FOUND", $search_arr, $count) ); 
  var_dump( $count );
  $i++;
}

?>
===DONE===
--EXPECT--	
*** Testing str_replace() with various search values ***
-- Iteration 0 --
array(12) {
  [0]=>
  unicode(5) "FOUND"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(5) "FOUND"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(6) "-FOUND"
  [5]=>
  unicode(5) "FOUND"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(6) "-FOUND"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(5)

-- Iteration 1 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(0)

-- Iteration 2 --
array(12) {
  [0]=>
  unicode(5) "FOUND"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(5) "FOUND"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(6) "-FOUND"
  [5]=>
  unicode(5) "FOUND"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(6) "-FOUND"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(5)

-- Iteration 3 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(5) "FOUND"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(5) "FOUND"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(2)

-- Iteration 4 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(5) "FOUND"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(5) "FOUND"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(2)

-- Iteration 5 --
array(12) {
  [0]=>
  unicode(5) "FOUND"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(5) "FOUND"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(6) "-FOUND"
  [5]=>
  unicode(5) "FOUND"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(6) "-FOUND"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(5)

-- Iteration 6 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(5) "FOUND"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(5) "FOUND"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(2)

-- Iteration 7 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(5) "FOUND"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(5) "FOUND"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(2)

-- Iteration 8 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(0)

-- Iteration 9 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(0)

-- Iteration 10 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(5) "FOUND"
  [11]=>
  unicode(0) ""
}
int(1)

-- Iteration 11 --
array(12) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(0) ""
  [2]=>
  unicode(1) "1"
  [3]=>
  unicode(1) "0"
  [4]=>
  unicode(2) "-1"
  [5]=>
  unicode(1) "1"
  [6]=>
  unicode(1) "0"
  [7]=>
  unicode(2) "-1"
  [8]=>
  unicode(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  unicode(3) "php"
  [11]=>
  unicode(0) ""
}
int(0)
===DONE===
