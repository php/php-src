--TEST--
Test abs() function : usage variations - different data types as $number arg
--FILE--
<?php
/*
 * Pass different data types as $number argument to abs() to test behaviour
 */

echo "*** Testing abs() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "abs";
  }
}

// heredoc string
$heredoc = <<<EOT
abs
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $number argument
$inputs = array(

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
/*19*/ "abs",
       'abs',
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

// loop through each element of $inputs to check the behavior of abs()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(abs($input));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};

fclose($fp);
?>
--EXPECTF--
*** Testing abs() : usage variations ***

-- Iteration 1 --

Deprecated: abs(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
int(0)

-- Iteration 2 --

Deprecated: abs(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
int(0)

-- Iteration 3 --
int(1)

-- Iteration 4 --
int(0)

-- Iteration 5 --
int(1)

-- Iteration 6 --
int(0)

-- Iteration 7 --
abs(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 8 --
abs(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 9 --
abs(): Argument #1 ($num) must be of type int|float, array given

-- Iteration 10 --
abs(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 11 --
abs(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 12 --
abs(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 13 --
abs(): Argument #1 ($num) must be of type int|float, classA given

-- Iteration 14 --

Deprecated: abs(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
int(0)

-- Iteration 15 --

Deprecated: abs(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
int(0)

-- Iteration 16 --
abs(): Argument #1 ($num) must be of type int|float, resource given
