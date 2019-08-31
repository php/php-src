--TEST--
Test fscanf() function: usage variations - scientific formats with integer values
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan different integer values using different scientific format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different scientific format types with different integer values ***\n";

// create a file
$filename = "$file_path/fscanf_variation45.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// different valid  integer values
$valid_ints = array(
  0,
  1,
  -1,
  -2147483648, // max negative integer value
  -2147483647,
  2147483647,  // max positive integer value
  2147483640,
  0x123B,      // integer as hexadecimal
  0x12ab,
  0Xfff,
  0XFA,
  -0x80000000, // max negative integer as hexadecimal
  0x7fffffff,  // max positive integer as hexadecimal
  0x7FFFFFFF,  // max positive integer as hexadecimal
  0123,        // integer as octal
  01,       // should be quivalent to octal 1
  -020000000000, // max negative integer as octal
  017777777777  // max positive integer as octal
);
// various scientific formats
$scientific_formats = array( "%e", "%he", "%le", "%Le", " %e", "%e ", "% e", "\t%e", "\n%e", "%4e", "%30e", "%[0-1]", "%*e");

$counter = 1;

// writing to the file
foreach($valid_ints as $int_value) {
  @fprintf($file_handle, $int_value);
  @fprintf($file_handle, "\n");
}
// closing the file
fclose($file_handle);

// opening the file for reading
$file_handle = fopen($filename, "r");
if($file_handle == false) {
  exit("Error:failed to open file $filename");
}

$counter = 1;
// reading the values from file using different scientific formats
foreach($scientific_formats as $scientific_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$scientific_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fscanf_variation45.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different scientific format types with different integer values ***

-- iteration 1 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
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
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  float(0)
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
  float(-214)
}
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
  float(2147)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
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
  float(2147)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-214)
}
array(1) {
  [0]=>
  float(2147)
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  float(0)
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
  float(-2147483648)
}
array(1) {
  [0]=>
  float(-2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483640)
}
array(1) {
  [0]=>
  float(4667)
}
array(1) {
  [0]=>
  float(4779)
}
array(1) {
  [0]=>
  float(4095)
}
array(1) {
  [0]=>
  float(250)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(2147483647)
}
array(1) {
  [0]=>
  float(83)
}
array(1) {
  [0]=>
  float(1)
}
array(1) {
  [0]=>
  float(-2147483648)
}
array(1) {
  [0]=>
  float(2147483647)
}
bool(false)

-- iteration 12 --
array(1) {
  [0]=>
  string(1) "0"
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
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
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
bool(false)

*** Done ***
