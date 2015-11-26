--TEST--
Test session_commit() function : error functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_commit(void)
 * Description : Write session data and end session
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_commit() : error functionality ***\n";

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
    var_dump(session_commit($input));
    $iterator++;
};

fclose($fp);
echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_commit() : error functionality ***

-- Iteration 1 --
NULL

-- Iteration 2 --
NULL

-- Iteration 3 --
NULL

-- Iteration 4 --
NULL

-- Iteration 5 --
NULL

-- Iteration 6 --
NULL

-- Iteration 7 --
NULL

-- Iteration 8 --
NULL

-- Iteration 9 --
NULL

-- Iteration 10 --
NULL

-- Iteration 11 --
NULL

-- Iteration 12 --
NULL

-- Iteration 13 --
NULL

-- Iteration 14 --
NULL

-- Iteration 15 --
NULL

-- Iteration 16 --
NULL

-- Iteration 17 --
NULL

-- Iteration 18 --
NULL

-- Iteration 19 --
NULL

-- Iteration 20 --
NULL

-- Iteration 21 --
NULL

-- Iteration 22 --
NULL

-- Iteration 23 --
NULL

-- Iteration 24 --
NULL
Done

