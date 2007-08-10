--TEST--
Test fscanf() function: usage variations - float formats with float values 
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan different float values using different format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different float format types with different float values ***\n"; 

// create a file
$filename = "$file_path/fscanf_variation8.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// different valid float vlaues
$valid_floats = array(
-2147483649, // float value
  2147483648,  // float value
  -0x80000001, // float value, beyond max negative int
  0x800000001, // float value, beyond max positive int
  020000000001, // float value, beyond max positive int
  -020000000001, // float value, beyond max negative int
  0.0,
  -0.1,
  10.0000000000000000005,
  10.5e+5,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
  1E5,
  -1E5,
  1E+5,
  -1E+5,
  1E-5,
  -1E-5,
  .5e+7,
  -.5e+7,
  .6e-19,
  -.6e-19,
  .05E+44,
  -.05E+44,
  .0034E-30,
  -.0034E-30
);

// various float formats
$float_formats = array( "%f", 
                        "%hf", "%lf", "%Lf",
                        " %f", "%f ", "% f",
                        "\t%f", "\n%f", "%4f",
                        "%30f", "%[0-9]", "%*f",
                 );

$counter = 1;

// writing to the file
foreach($valid_floats as $float_value) {
  fprintf($file_handle, $float_value);
  fprintf($file_handle, "\n");
}
// closing the file
fclose($file_handle);

// opening the file for reading
$file_handle = fopen($filename, "r");
if($file_handle == false) {
  exit("Error:failed to open file $filename");
}

$counter = 1;
// reading the values from file using different float formats
foreach($float_formats as $float_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$float_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_variation8.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different float format types with different float values ***

-- iteration 1 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 7 --

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL

Warning: fscanf(): Bad scan conversion character " " in %s on line %d
NULL
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  float(-214)
}
array(1) {
  [0]=>
  float(2147)
}
array(1) {
  [0]=>
  float(-214)
}
array(1) {
  [0]=>
  float(3435)
}
array(1) {
  [0]=>
  float(2147)
}
array(1) {
  [0]=>
  float(-214)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050)
}
array(1) {
  [0]=>
  float(1000)
}
array(1) {
  [0]=>
  float(-100)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-1)
}
array(1) {
  [0]=>
  float(1000)
}
array(1) {
  [0]=>
  float(-100)
}
array(1) {
  [0]=>
  float(1000)
}
array(1) {
  [0]=>
  float(-100)
}
array(1) {
  [0]=>
  float(1000)
}
array(1) {
  [0]=>
  float(-100)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-1)
}
array(1) {
  [0]=>
  float(5000)
}
array(1) {
  [0]=>
  float(-500)
}
array(1) {
  [0]=>
  float(6)
}
array(1) {
  [0]=>
  float(-6)
}
array(1) {
  [0]=>
  float(5)
}
array(1) {
  [0]=>
  float(-5)
}
array(1) {
  [0]=>
  float(3.4)
}
array(1) {
  [0]=>
  float(-3.4)
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(2147483648)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(34359738369)
}
array(1) {
  [0]=>
  float(2147483649)
}
array(1) {
  [0]=>
  float(-2147483649)
}
array(1) {
  [0]=>
  float(0)
}
array(1) {
  [0]=>
  float(-0.1)
}
array(1) {
  [0]=>
  float(10)
}
array(1) {
  [0]=>
  float(1050000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(100000)
}
array(1) {
  [0]=>
  float(-100000)
}
array(1) {
  [0]=>
  float(1.0E-5)
}
array(1) {
  [0]=>
  float(-1.0E-5)
}
array(1) {
  [0]=>
  float(5000000)
}
array(1) {
  [0]=>
  float(-5000000)
}
array(1) {
  [0]=>
  float(6.0E-20)
}
array(1) {
  [0]=>
  float(-6.0E-20)
}
array(1) {
  [0]=>
  float(5.0E+42)
}
array(1) {
  [0]=>
  float(-5.0E+42)
}
array(1) {
  [0]=>
  float(3.4E-33)
}
array(1) {
  [0]=>
  float(-3.4E-33)
}
bool(false)

-- iteration 12 --
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(11) "34359738369"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(2) "10"
}
array(1) {
  [0]=>
  string(7) "1050000"
}
array(1) {
  [0]=>
  string(6) "100000"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(6) "100000"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(6) "100000"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(6) "100000"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(7) "5000000"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "6"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "5"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  NULL
}
bool(false)

-- iteration 13 --
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
bool(false)

*** Done ***
