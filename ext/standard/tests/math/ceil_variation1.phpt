--TEST--
Test ceil() function : usage variations - different data types as $value arg
--INI--
precision=14
--FILE--
<?php
echo "*** Testing ceil() : usage variations ***\n";
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

// loop through each element of $inputs to check the behaviour of ceil()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(ceil($input));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};
fclose($fp);
?>
--EXPECT--
*** Testing ceil() : usage variations ***

-- Iteration 1 --
float(0)

-- Iteration 2 --
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
ceil(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 8 --
ceil(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 9 --
ceil(): Argument #1 ($num) must be of type int|float, array given

-- Iteration 10 --
ceil(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 11 --
ceil(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 12 --
ceil(): Argument #1 ($num) must be of type int|float, string given

-- Iteration 13 --
ceil(): Argument #1 ($num) must be of type int|float, classA given

-- Iteration 14 --
float(0)

-- Iteration 15 --
float(0)

-- Iteration 16 --
ceil(): Argument #1 ($num) must be of type int|float, resource given
