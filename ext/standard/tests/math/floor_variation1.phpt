--TEST--
Test floor() function : usage variations - different data types as $value arg
--INI--
precision=14
--FILE--
<?php
echo "*** Testing floor() : usage variations ***\n";
//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
}

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $value argument
$inputs = array(
       // null data
/* 1*/ NULL,
       null,

       // boolean data
/* 3*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/* 7*/ "",
       '',
       array(),

       // string data
/*10*/ "abcxyz",
       'abcxyz}',
       $heredoc,

       // object data
/*13*/ new classA(),

       // undefined data
/*14*/ @$undefined_var,

       // unset data
/*15*/ @$unset_var,

       // resource variable
/*16*/ $fp
);

// loop through each element of $inputs to check the behaviour of floor()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(floor($input));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};
fclose($fp);
?>
--EXPECTF--
*** Testing floor() : usage variations ***

-- Iteration 1 --

Deprecated: floor(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 2 --

Deprecated: floor(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 3 --
float(1)

-- Iteration 4 --
float(0)

-- Iteration 5 --
float(1)

-- Iteration 6 --
float(0)

-- Iteration 7 --
floor(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 8 --
floor(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 9 --
floor(): Argument #1 ($num) must be of type int|float, array given

-- Iteration 10 --
floor(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 11 --
floor(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 12 --
floor(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 13 --
floor(): Argument #1 ($num) must be of type int|float, classA given

-- Iteration 14 --

Deprecated: floor(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 15 --

Deprecated: floor(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)

-- Iteration 16 --
floor(): Argument #1 ($num) must be of type int|float, resource given
