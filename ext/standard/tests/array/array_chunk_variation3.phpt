--TEST--
Test array_chunk() function : usage variations - unexpected values for 'preserve_keys' 
--FILE--
<?php
/* Prototype  : array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks 
 * Source code: ext/standard/array.c
*/

/*
* Testing array_chunk() function with unexpected values for 'preserve_keys' 
*/

echo "*** Testing array_chunk() : usage variations ***\n";

// input array
$input = array(1, 2);
$size = 10;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array(

        // int data
/*1*/   0,
        1,
        12345,
        -2345,

        // float data
/*5*/   10.5,
        -10.5,
        10.5e10,
        10.6E-10,
        .5,

        // null data
/*10*/  NULL,
        null,

        // empty data
/*12*/  "",
        '',

        // string data
/*14*/  "string",
        'string',

        // object data
/*16*/  new stdclass(),

        // undefined data
/*17*/  @undefined_var,

        // unset data
/*18*/  @unset_var

);

$count = 1;

// loop through each element of the array for preserve_keys
foreach($values as $value) {
  echo "\n-- Iteration $count --\n";
  var_dump( array_chunk($input, $size, $value) );
  $count++;
}

echo "Done";
?>
--EXPECTF--
*** Testing array_chunk() : usage variations ***

-- Iteration 1 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 2 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 3 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 4 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 5 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 6 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 7 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 8 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 9 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 10 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 11 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 12 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 13 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 14 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 15 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 16 --

Warning: array_chunk() expects parameter 3 to be boolean, object given in %s on line %d
NULL

-- Iteration 17 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}

-- Iteration 18 --
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
Done
