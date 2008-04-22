--TEST--
Test session_register() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_register(mixed $name [,mixed $...])
 * Description : Register one or more global variables with the current session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_register() : basic functionality ***\n";

// Get an unset variable
$unset_var = 10;
unset($unset_var);

class classA
{
    public function __toString() {
        return "Hello World!";
    }
}

$heredoc = <<<EOT
Hello World!
EOT;

$fp = fopen(__FILE__, "r");

// Unexpected values to be passed as arguments
$inputs = array(

       // Integer data
/*1*/  0,
       1,
       12345,
       -2345,

       // Float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // Null data
/*10*/ NULL,
       null,

       // Boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
       
       // Empty strings
/*16*/ "",
       '',

       // Invalid string data
/*18*/ "Nothing",
       'Nothing',
       $heredoc,
       
       // Object data
/*21*/ new classA(),

       // Undefined data
/*22*/ @$undefined_var,

       // Unset data
/*23*/ @$unset_var,

       // Resource variable
/*24*/ $fp
);


$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(session_start());
    var_dump(session_register($input));
    var_dump($_SESSION);
    var_dump(session_destroy());
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_register() : basic functionality ***

-- Iteration 1 --
bool(true)
bool(true)
array(1) {
  ["0"]=>
  NULL
}
bool(true)

-- Iteration 2 --
bool(true)
bool(true)
array(1) {
  ["1"]=>
  NULL
}
bool(true)

-- Iteration 3 --
bool(true)
bool(true)
array(1) {
  ["12345"]=>
  NULL
}
bool(true)

-- Iteration 4 --
bool(true)
bool(true)
array(1) {
  ["-2345"]=>
  NULL
}
bool(true)

-- Iteration 5 --
bool(true)
bool(true)
array(1) {
  ["10.5"]=>
  NULL
}
bool(true)

-- Iteration 6 --
bool(true)
bool(true)
array(1) {
  ["-10.5"]=>
  NULL
}
bool(true)

-- Iteration 7 --
bool(true)
bool(true)
array(1) {
  ["123456789000"]=>
  NULL
}
bool(true)

-- Iteration 8 --
bool(true)
bool(true)
array(1) {
  ["1.23456789E-9"]=>
  NULL
}
bool(true)

-- Iteration 9 --
bool(true)
bool(true)
array(1) {
  ["0.5"]=>
  NULL
}
bool(true)

-- Iteration 10 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 11 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 12 --
bool(true)
bool(true)
array(1) {
  ["1"]=>
  NULL
}
bool(true)

-- Iteration 13 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 14 --
bool(true)
bool(true)
array(1) {
  ["1"]=>
  NULL
}
bool(true)

-- Iteration 15 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 16 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 17 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 18 --
bool(true)
bool(true)
array(1) {
  ["Nothing"]=>
  NULL
}
bool(true)

-- Iteration 19 --
bool(true)
bool(true)
array(1) {
  ["Nothing"]=>
  NULL
}
bool(true)

-- Iteration 20 --
bool(true)
bool(true)
array(1) {
  ["Hello World!"]=>
  NULL
}
bool(true)

-- Iteration 21 --
bool(true)
bool(true)
array(1) {
  ["Hello World!"]=>
  NULL
}
bool(true)

-- Iteration 22 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 23 --
bool(true)
bool(true)
array(1) {
  [""]=>
  NULL
}
bool(true)

-- Iteration 24 --
bool(true)
bool(true)
array(1) {
  ["Resource id #%d"]=>
  NULL
}
bool(true)
Done

