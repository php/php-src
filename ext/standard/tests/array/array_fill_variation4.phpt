--TEST--
Test array_fill() function : usage variations - using return value of array_fill for 'val' argument
--FILE--
<?php
/* passing array_fill() as the 'val' argument in array_fill() function */

echo "*** Testing array_fill() : variation ***\n";

$start_key = 0;
$num = 2;
$heredoc = <<<HERE_DOC
Hello
HERE_DOC;

// array of possible valid values for 'val' argument
$values = array (

  /* 1  */  NULL,
            0,
            1,
  /* 4  */  1.0,
            'hi',
            "hi",
  /* 7  */  $heredoc
);

echo "*** Filling 2 dimensional array with all basic valid values ***\n";
$counter = 1;
for($i =0; $i < count($values); $i ++)
{
  echo "-- Iteration $counter --\n";
  $val = $values[$i];

  var_dump( array_fill($start_key,$num,array_fill($start_key,$num,$val)) );

  $counter++;
}

echo "Done";
?>
--EXPECT--
*** Testing array_fill() : variation ***
*** Filling 2 dimensional array with all basic valid values ***
-- Iteration 1 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
}
-- Iteration 2 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
}
-- Iteration 3 --
array(2) {
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
    int(1)
    [1]=>
    int(1)
  }
}
-- Iteration 4 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    float(1)
    [1]=>
    float(1)
  }
  [1]=>
  array(2) {
    [0]=>
    float(1)
    [1]=>
    float(1)
  }
}
-- Iteration 5 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "hi"
    [1]=>
    string(2) "hi"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "hi"
    [1]=>
    string(2) "hi"
  }
}
-- Iteration 6 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "hi"
    [1]=>
    string(2) "hi"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "hi"
    [1]=>
    string(2) "hi"
  }
}
-- Iteration 7 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "Hello"
    [1]=>
    string(5) "Hello"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "Hello"
    [1]=>
    string(5) "Hello"
  }
}
Done
