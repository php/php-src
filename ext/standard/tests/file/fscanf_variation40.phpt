--TEST--
Test fscanf() function: usage variations - unsigned formats with float values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
	die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan float values using different unsigned format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different unsigned format types with float values ***\n";

// create a file
$filename = "$file_path/fscanf_variation40.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array of float type values

$float_values = array (
  -2147483649,
  2147483648,
  -0x80000001, // float value, beyond max negative int
  0x800000001, // float value, beyond max positive int
  020000000001, // float value, beyond max positive int
  -020000000001, // float value, beyond max negative int
  0.0,
  -0.1,
  1.0,
  1e5,
  -1e6,
  1E8,
  -1E9,
  10.0000000000000000005,
  10.5e+5
);

$unsigned_formats = array( "%u", "%hu", "%lu", "%Lu", " %u", "%u ", "% u", "\t%u", "\n%u", "%4u", "%30u", "%[0-9]", "%*u");

$counter = 1;

// writing to the file
foreach($float_values as $value) {
  @fprintf($file_handle, $value);
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
// reading the values from file using different unsigned formats
foreach($unsigned_formats as $unsigned_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$unsigned_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_variation40.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different unsigned format types with float values ***

-- iteration 1 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
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
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  string(10) "4294967082"
}
array(1) {
  [0]=>
  int(2147)
}
array(1) {
  [0]=>
  string(10) "4294967082"
}
array(1) {
  [0]=>
  int(3435)
}
array(1) {
  [0]=>
  int(2147)
}
array(1) {
  [0]=>
  string(10) "4294967082"
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1000)
}
array(1) {
  [0]=>
  string(10) "4294967196"
}
array(1) {
  [0]=>
  int(1000)
}
array(1) {
  [0]=>
  string(10) "4294967196"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050)
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483649"
}
array(1) {
  [0]=>
  int(2147483647)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(100000)
}
array(1) {
  [0]=>
  string(10) "4293967296"
}
array(1) {
  [0]=>
  int(100000000)
}
array(1) {
  [0]=>
  string(10) "3294967296"
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1050000)
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
  string(1) "1"
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
  string(9) "100000000"
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
bool(false)

*** Done ***
