--TEST--
Test ctype_xdigit() function : usage variations - different data typse as $c arg
--EXTENSIONS--
ctype
--FILE--
<?php
/*
 * Pass different data types as $c argument to ctype_xdigit() to test behaviour
 */

echo "*** Testing ctype_xdigit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "ABcd";
  }
}

// heredoc string
$heredoc = <<<EOT
234
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $c argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',
       array(),

       // string data
/*19*/ "456",
       'def',
       $heredoc,

       // object data
/*22*/ new classA(),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of ctype_xdigit()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump( ctype_xdigit($input) );
    $iterator++;
};

fclose($fp);

setlocale(LC_CTYPE, $orig);
?>
--EXPECTF--
*** Testing ctype_xdigit() : usage variations ***

-- Iteration 1 --

Deprecated: ctype_xdigit(): Argument of type int will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 2 --

Deprecated: ctype_xdigit(): Argument of type int will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 3 --

Deprecated: ctype_xdigit(): Argument of type int will be interpreted as string in the future in %s on line %d
bool(true)

-- Iteration 4 --

Deprecated: ctype_xdigit(): Argument of type int will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 5 --

Deprecated: ctype_xdigit(): Argument of type float will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 6 --

Deprecated: ctype_xdigit(): Argument of type float will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 7 --

Deprecated: ctype_xdigit(): Argument of type float will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 8 --

Deprecated: ctype_xdigit(): Argument of type float will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 9 --

Deprecated: ctype_xdigit(): Argument of type float will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 10 --

Deprecated: ctype_xdigit(): Argument of type null will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 11 --

Deprecated: ctype_xdigit(): Argument of type null will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 12 --

Deprecated: ctype_xdigit(): Argument of type bool will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 13 --

Deprecated: ctype_xdigit(): Argument of type bool will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 14 --

Deprecated: ctype_xdigit(): Argument of type bool will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 15 --

Deprecated: ctype_xdigit(): Argument of type bool will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --

Deprecated: ctype_xdigit(): Argument of type array will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 19 --
bool(true)

-- Iteration 20 --
bool(true)

-- Iteration 21 --
bool(true)

-- Iteration 22 --

Deprecated: ctype_xdigit(): Argument of type classA will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 23 --

Deprecated: ctype_xdigit(): Argument of type null will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 24 --

Deprecated: ctype_xdigit(): Argument of type null will be interpreted as string in the future in %s on line %d
bool(false)

-- Iteration 25 --

Deprecated: ctype_xdigit(): Argument of type resource will be interpreted as string in the future in %s on line %d
bool(false)
