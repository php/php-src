--TEST--
Test array_fill() function : usage variations - different types of array values for 'val' argument
--FILE--
<?php
/* Prototype  : array array_fill(int $start_key, int $num, mixed $val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val 
 * Source code: ext/standard/array.c
 */

/*
 * testing array_fill() by passing different types of array  values for 'val' argument
 */

echo "*** Testing array_fill() : usage variations ***\n";

// Initialise function arguments not being substituted 
$start_key = 0;
$num = 2;


//array of different types of array values for 'val' argument  
$values = array(
           
  /* 1  */  array(),
            array(1 , 2 , 3 , 4),
            array(1 => "Hi" , 2 => "Hello"),
            array("Saffron" , "White" , "Green"),
  /* 5  */  array('color' => 'red' , 'item' => 'pen'),
            array( 'color' => 'red' , 2 => 'green ' ),
            array("colour" => "red" , "item" => "pen"),
            array( TRUE => "red" , FALSE => "green" ),
            array( true => "red" , FALSE => "green" ),
  /* 10 */  array( 1 => "Hi" , "color" => "red" , 'item' => 'pen'),
            array( NULL => "Hi", '1' => "Hello" , "1" => "Green"),
            array( ""=>1, "color" => "green"), 
  /* 13 */  array('Saffron' , 'White' , 'Green')
);

// loop through each element of the values array for 'val' argument 
// check the working of array_fill()
echo "--- Testing array_fill() with different types of array values for 'val' argument ---\n";
$counter = 1;
for($i = 0; $i < count($values); $i++)
{
  echo "-- Iteration $counter --\n";
  $val = $values[$i];
  
  var_dump( array_fill($start_key , $num , $val) );

  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_fill() : usage variations ***
--- Testing array_fill() with different types of array values for 'val' argument ---
-- Iteration 1 --
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
-- Iteration 2 --
array(2) {
  [0]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [1]=>
  array(4) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
}
-- Iteration 3 --
array(2) {
  [0]=>
  array(2) {
    [1]=>
    string(2) "Hi"
    [2]=>
    string(5) "Hello"
  }
  [1]=>
  array(2) {
    [1]=>
    string(2) "Hi"
    [2]=>
    string(5) "Hello"
  }
}
-- Iteration 4 --
array(2) {
  [0]=>
  array(3) {
    [0]=>
    string(7) "Saffron"
    [1]=>
    string(5) "White"
    [2]=>
    string(5) "Green"
  }
  [1]=>
  array(3) {
    [0]=>
    string(7) "Saffron"
    [1]=>
    string(5) "White"
    [2]=>
    string(5) "Green"
  }
}
-- Iteration 5 --
array(2) {
  [0]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
  [1]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
}
-- Iteration 6 --
array(2) {
  [0]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    [2]=>
    string(6) "green "
  }
  [1]=>
  array(2) {
    ["color"]=>
    string(3) "red"
    [2]=>
    string(6) "green "
  }
}
-- Iteration 7 --
array(2) {
  [0]=>
  array(2) {
    ["colour"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
  [1]=>
  array(2) {
    ["colour"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
}
-- Iteration 8 --
array(2) {
  [0]=>
  array(2) {
    [1]=>
    string(3) "red"
    [0]=>
    string(5) "green"
  }
  [1]=>
  array(2) {
    [1]=>
    string(3) "red"
    [0]=>
    string(5) "green"
  }
}
-- Iteration 9 --
array(2) {
  [0]=>
  array(2) {
    [1]=>
    string(3) "red"
    [0]=>
    string(5) "green"
  }
  [1]=>
  array(2) {
    [1]=>
    string(3) "red"
    [0]=>
    string(5) "green"
  }
}
-- Iteration 10 --
array(2) {
  [0]=>
  array(3) {
    [1]=>
    string(2) "Hi"
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
  [1]=>
  array(3) {
    [1]=>
    string(2) "Hi"
    ["color"]=>
    string(3) "red"
    ["item"]=>
    string(3) "pen"
  }
}
-- Iteration 11 --
array(2) {
  [0]=>
  array(2) {
    [""]=>
    string(2) "Hi"
    [1]=>
    string(5) "Green"
  }
  [1]=>
  array(2) {
    [""]=>
    string(2) "Hi"
    [1]=>
    string(5) "Green"
  }
}
-- Iteration 12 --
array(2) {
  [0]=>
  array(2) {
    [""]=>
    int(1)
    ["color"]=>
    string(5) "green"
  }
  [1]=>
  array(2) {
    [""]=>
    int(1)
    ["color"]=>
    string(5) "green"
  }
}
-- Iteration 13 --
array(2) {
  [0]=>
  array(3) {
    [0]=>
    string(7) "Saffron"
    [1]=>
    string(5) "White"
    [2]=>
    string(5) "Green"
  }
  [1]=>
  array(3) {
    [0]=>
    string(7) "Saffron"
    [1]=>
    string(5) "White"
    [2]=>
    string(5) "Green"
  }
}
Done
