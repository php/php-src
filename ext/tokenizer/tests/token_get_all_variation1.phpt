--TEST--
Test token_get_all() function : usage variations - unexpected values for 'source' argument
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : array token_get_all(string $source)
 * Description: splits the given source into an array of PHP languange tokens 
 * Source code: ext/tokenizer/tokenizer.c
*/

/*
 * Passing different scalar/non-scalar values in place of 'source' argument
 *   It returns either T_INLINE_HTML by converting values into string or gives warning
*/

echo "*** Testing token_get_all() : unexpected values for 'source' argument ***\n";

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// class definition
class MyClass
{
  public function __toString()
  {
    return "object";
  }
}

// get resource
$fp = fopen(__FILE__, 'r');

// different scalar/nonscalar values for 'source'
$source_values = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       10.1234567e8,
       10.7654321E-8,
       .5,

       // array data
/*10*/ array(),
       array(0),
       array(1),
       array(1, 2),
       array('color' => 'red', 'item' => 'pen'),

       // null data
/*15*/ NULL,
       null,

       // boolean data
/*17*/ true,
       false,
       TRUE,
       FALSE,

       // empty string
/*21*/ "",
       '',

       // object data
/*23*/ new MyClass(),
 
       // resource data
       $fp,

       // undefined data
       @$undefined_var,

       // unset data
/*26*/ @$unset_var,
);

for($count = 0; $count < count($source_values); $count++) {
  echo "--Iteration ".($count + 1)." --\n";
  var_dump( token_get_all($source_values[$count]));
};

fclose($fp);
echo "Done"
?>
--EXPECTF--
*** Testing token_get_all() : unexpected values for 'source' argument ***
--Iteration 1 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "0"
    [2]=>
    int(1)
  }
}
--Iteration 2 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
}
--Iteration 3 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "12345"
    [2]=>
    int(1)
  }
}
--Iteration 4 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "-2345"
    [2]=>
    int(1)
  }
}
--Iteration 5 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(4) "10.5"
    [2]=>
    int(1)
  }
}
--Iteration 6 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(5) "-10.5"
    [2]=>
    int(1)
  }
}
--Iteration 7 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(10) "1012345670"
    [2]=>
    int(1)
  }
}
--Iteration 8 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(13) "1.07654321E-7"
    [2]=>
    int(1)
  }
}
--Iteration 9 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(3) "0.5"
    [2]=>
    int(1)
  }
}
--Iteration 10 --

Warning: token_get_all() expects parameter 1 to be string, array given in %s on line %d
NULL
--Iteration 11 --

Warning: token_get_all() expects parameter 1 to be string, array given in %s on line %d
NULL
--Iteration 12 --

Warning: token_get_all() expects parameter 1 to be string, array given in %s on line %d
NULL
--Iteration 13 --

Warning: token_get_all() expects parameter 1 to be string, array given in %s on line %d
NULL
--Iteration 14 --

Warning: token_get_all() expects parameter 1 to be string, array given in %s on line %d
NULL
--Iteration 15 --
array(0) {
}
--Iteration 16 --
array(0) {
}
--Iteration 17 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
}
--Iteration 18 --
array(0) {
}
--Iteration 19 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(1) "1"
    [2]=>
    int(1)
  }
}
--Iteration 20 --
array(0) {
}
--Iteration 21 --
array(0) {
}
--Iteration 22 --
array(0) {
}
--Iteration 23 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    string(6) "object"
    [2]=>
    int(1)
  }
}
--Iteration 24 --

Warning: token_get_all() expects parameter 1 to be string, resource given in %s on line %d
NULL
--Iteration 25 --
array(0) {
}
--Iteration 26 --
array(0) {
}
Done
