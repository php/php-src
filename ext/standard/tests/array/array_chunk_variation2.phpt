--TEST--
Test array_chunk() function : usage variations - unexpected values for 'size' argument
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks
              : Chunks an array into size  large chunks
 * Source code: ext/standard/array.c
*/

/*
* Testing array_chunk() function with unexpected values for 'size' argument
*/

echo "*** Testing array_chunk() : usage variations ***\n";

// input array
$input = array(1, 2);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//array of values to iterate over
$values = array (

        // float data
/*1*/   10.5,
        -10.5,
        10.5e10,
        10.6E-10,
        .5,

        // array data
/*6*/   array(),
        array(0),
        array(1),
        array(1, 2),
        array('color' => 'red', 'item' => 'pen'),

        // null data
/*11*/  NULL,
        null,

        // boolean data
/*13*/  true,
        false,
        TRUE,
        FALSE,

        // empty data
/*17*/  "",
        '',

        // string data
/*19*/  "string",
        'string',

        // object data
/*21*/  new stdclass(),

        // undefined data
/*22*/  @$undefined_var,

        // unset data
/*23*/  @$unset_var

);

// loop through each element of the array for size
$count = 1;
foreach($values as $value){
  echo "\n-- Iteration $count --\n";
  var_dump( array_chunk($input, $value) );
  var_dump( array_chunk($input, $value, true) );
  var_dump( array_chunk($input, $value, false) );
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
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
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

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

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
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
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

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 5 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 6 --

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

-- Iteration 7 --

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

-- Iteration 8 --

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

-- Iteration 9 --

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

-- Iteration 10 --

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, array given in %s on line %d
NULL

-- Iteration 11 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 12 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 13 --
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [1]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}

-- Iteration 14 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 15 --
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [1]=>
    int(2)
  }
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
}

-- Iteration 16 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 17 --

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: array_chunk() expects parameter 2 to be int, object given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, object given in %s on line %d
NULL

Warning: array_chunk() expects parameter 2 to be int, object given in %s on line %d
NULL

-- Iteration 22 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

-- Iteration 23 --

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL

Warning: array_chunk(): Size parameter expected to be greater than 0 in %s on line %d
NULL
Done
