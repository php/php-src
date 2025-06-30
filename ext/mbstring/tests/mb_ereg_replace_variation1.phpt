--TEST--
Test mb_ereg_replace() function : usage variations  - different input types
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
echo "*** Testing mb_ereg_replace() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$replacement = 'string_val';
$string = 'string_val';
$option = '';

// unexpected values to be passed to $encoding argument
$inputs = [
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
       '',
       'UTF-8',
];

// loop through each element of the array for pattern

$iterator = 1;
foreach($inputs as $input) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( mb_ereg_replace($input, $replacement, $string, $option) );
      $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing mb_ereg_replace() : usage variations ***

-- Iteration 1 --
string(10) "string_val"

-- Iteration 2 --
string(10) "string_val"

-- Iteration 3 --
string(10) "string_val"

-- Iteration 4 --
string(10) "string_val"

-- Iteration 5 --
string(10) "string_val"

-- Iteration 6 --
string(10) "string_val"

-- Iteration 7 --
string(10) "string_val"

-- Iteration 8 --
string(10) "string_val"

-- Iteration 9 --
string(10) "string_val"

-- Iteration 10 --
string(120) "string_valsstring_valtstring_valrstring_valistring_valnstring_valgstring_val_string_valvstring_valastring_vallstring_val"

-- Iteration 11 --
string(10) "string_val"
Done
