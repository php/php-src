--TEST--
Test array_map() function
--FILE--
<?php
/* Prototype: array array_map ( callback $callback, array $arr1 [, array $...] );
   Description: array_map() returns an array containing all the elements of arr1 
                after applying the callback function to each one. The number of 
                parameters that the callback function accepts should match the 
                number of arrays passed to the array_map() 
*/

echo "*** Testing basic operations ***\n";
/* array_map with null as function and different arrays */
var_dump( array_map(NULL, array()) );
var_dump( array_map(NULL, array(), array()) );
var_dump( array_map(NULL, array(1,2,3), array(1,2,3)) ); 
var_dump( array_map(NULL, array(1,2), array(1,2,3,4)) );
var_dump( array_map(NULL, array("Jan", "Feb", "March"), array("31","28","31")) ); 
var_dump( array_map(NULL, array("Text", "Words", "Lineup"), array(4, 5, 6)) ); 
var_dump( array_map(NULL, array("a", "ab", "abc", "abcd"), array()) ); 
var_dump( array_map(NULL, 
                    array("Jan"=>"01", "Feb"=>"02", "March"=>"03"), 
                    array("31"=>"Jan", "28"=>"Feb", "031"=>"March") 
                   )
        ); 

/* using key as "string" where no.of arguments passed to array_map() is 2 */
var_dump( array_map( create_function('$n', 'return $n*$n;'),
                     array("key1"=>1, "key2"=>2, "key3"=>3)
                   )
        );

echo "\n*** Testing possible variations ***\n";
/* anonymous callback function */
var_dump( array_map( create_function('$a,$b', 'return $a+$b;'), 
                     array(1,2,3), 
                     array(5,6,7,8,9) 
                   ) 
        );

/* anonymous callback function with reference */
var_dump( array_map( create_function('&$a, $b', 'return array($a,$b);'), 
                     array("Hello","Good"), 
                     array("World","Day")
                   )
        );

/* callback function with reference */
$a = array(1,2,3);
function square(&$var) {
  return( $var * $var );
} 
print_r( array_map('square', $a) );

/* array_map in recursion */
function square_recur($var) {
   if (is_array($var))
     return array_map('square_recur', $var);
   return $var * $var;
}
$rec_array = array(1, 2, array(3, 4, array(5, 2), array() ) );
var_dump( array_map('square_recur', $rec_array) );

/* callback function as string variable containing the function name */
$string_var = "square";
var_dump( array_map("square", $a) );
var_dump( array_map($string_var, $a) ); 

echo "\n*** Testing error conditions ***\n";
/* arguments of non array type */
$int_var=10;
$float_var = 10.5;
var_dump( array_map('square', $int_var) );
var_dump( array_map('square', $float_var) );
var_dump( array_map('square', $string_var) );

/* Zero argument */
var_dump( array_map() );

/* use array(), echo(), empty(), eval(), exit(), isset(), list(), print()
   and unset() as callback, failure expected */
var_dump( array_map( 'echo', array(1) ) );
var_dump( array_map( 'array', array(1) ) );
var_dump( array_map( 'empty', array(1) ) );
var_dump( array_map( 'eval', array(1) ) );
var_dump( array_map( 'exit', array(1) ) );
var_dump( array_map( 'isset', array(1) ) );
var_dump( array_map( 'list', array(1) ) );
var_dump( array_map( 'print', array(1) ) );


echo "\n*** Testing operation on objects ***\n";
/* array_map with class object */
class check_array_map {
  public static function helloWorld() {
    return "Static_Function_helloWorld(): Hello World";
  }
  public function Message($v) {
    return $v;
  }
  
  public static function Square( $n ) {
    return $n * $n;
  }
} 
/* call static member function */
var_dump( array_map( array('check_array_map', 'Square'), array(1,2,3)) );

/* call non static member function - notice should be issues*/
var_dump( array_map( array('check_array_map', 'Message'), array(1)) );  

/* call function using object */
$obj = new check_array_map(); 
var_dump( array_map( array($obj, 'helloWorld' ) ) ); // not enough args warning
var_dump( array_map( array($obj, 'helloWorld'), array(1) ) );

echo "Done\n";
?>
--EXPECTF--	
*** Testing basic operations ***
array(0) {
}
array(0) {
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(2)
  }
  [2]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(3)
  }
}
array(4) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
  [1]=>
  array(2) {
    [0]=>
    int(2)
    [1]=>
    int(2)
  }
  [2]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    int(4)
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "Jan"
    [1]=>
    string(2) "31"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "Feb"
    [1]=>
    string(2) "28"
  }
  [2]=>
  array(2) {
    [0]=>
    string(5) "March"
    [1]=>
    string(2) "31"
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "Text"
    [1]=>
    int(4)
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "Words"
    [1]=>
    int(5)
  }
  [2]=>
  array(2) {
    [0]=>
    string(6) "Lineup"
    [1]=>
    int(6)
  }
}
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "ab"
    [1]=>
    NULL
  }
  [2]=>
  array(2) {
    [0]=>
    string(3) "abc"
    [1]=>
    NULL
  }
  [3]=>
  array(2) {
    [0]=>
    string(4) "abcd"
    [1]=>
    NULL
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "01"
    [1]=>
    string(3) "Jan"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "02"
    [1]=>
    string(3) "Feb"
  }
  [2]=>
  array(2) {
    [0]=>
    string(2) "03"
    [1]=>
    string(5) "March"
  }
}
array(3) {
  ["key1"]=>
  int(1)
  ["key2"]=>
  int(4)
  ["key3"]=>
  int(9)
}

*** Testing possible variations ***
array(5) {
  [0]=>
  int(6)
  [1]=>
  int(8)
  [2]=>
  int(10)
  [3]=>
  int(8)
  [4]=>
  int(9)
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "Hello"
    [1]=>
    string(5) "World"
  }
  [1]=>
  array(2) {
    [0]=>
    string(4) "Good"
    [1]=>
    string(3) "Day"
  }
}
Array
(
    [0] => 1
    [1] => 4
    [2] => 9
)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  array(4) {
    [0]=>
    int(9)
    [1]=>
    int(16)
    [2]=>
    array(2) {
      [0]=>
      int(25)
      [1]=>
      int(4)
    }
    [3]=>
    array(0) {
    }
  }
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(9)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(9)
}

*** Testing error conditions ***

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL

Warning: array_map(): Argument #2 should be an array in %s on line %d
NULL

Warning: Wrong parameter count for array_map() %s on line %d
NULL

Warning: array_map(): The first argument, 'echo', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'array', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'empty', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'eval', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'exit', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'isset', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'list', should be either NULL or a valid callback in %s on line %d
NULL

Warning: array_map(): The first argument, 'print', should be either NULL or a valid callback in %s on line %d
NULL

*** Testing operation on objects ***
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(9)
}

Strict Standards: Non-static method check_array_map::Message() cannot be called statically in %s on line %d

Strict Standards: Non-static method check_array_map::Message() cannot be called statically in %s on line %d
array(1) {
  [0]=>
  int(1)
}

Warning: Wrong parameter count for array_map() in %s on line %d
NULL
array(1) {
  [0]=>
  string(41) "Static_Function_helloWorld(): Hello World"
}
Done
