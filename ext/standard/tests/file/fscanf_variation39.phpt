--TEST--
Test fscanf() function: usage variations - unsigned int formats with integer values 
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

/* Test fscanf() to scan different integer values using different unsigned int format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different unsigned int format types with different integer values ***\n"; 

// create a file
$filename = "$file_path/fscanf_variation39.tmp";
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
  0x7fffffff,  // max postive integer as hexadecimal
  0x7FFFFFFF,  // max postive integer as hexadecimal
  0123,        // integer as octal
  01912,       // should be quivalent to octal 1
  -020000000000, // max negative integer as octal
  017777777777  // max positive integer as octal
);
// various unsigned int formats
$unsigned_formats = array( "%u", "%hu", "%lu", "%Lu", " %u", "%u ", "% u", "\t%u", "\n%u", "%4u", "%30u", "%[0-9]", "%*u");

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
// reading the values from file using different unsigned int formats
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
$filename = "$file_path/fscanf_variation39.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different unsigned int format types with different integer values ***

-- iteration 1 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 2 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 3 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 4 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 5 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 6 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
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
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 9 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 10 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "4294967082"
}
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
  int(2147)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
}
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
  int(2147)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "4294967082"
}
array(1) {
  [0]=>
  int(2147)
}
bool(false)

-- iteration 11 --
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
  string(10) "4294967295"
}
array(1) {
  [0]=>
  string(10) "2147483648"
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
  int(2147483640)
}
array(1) {
  [0]=>
  int(4667)
}
array(1) {
  [0]=>
  int(4779)
}
array(1) {
  [0]=>
  int(4095)
}
array(1) {
  [0]=>
  int(250)
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
  int(2147483647)
}
array(1) {
  [0]=>
  int(83)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  int(2147483647)
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
  string(10) "2147483647"
}
array(1) {
  [0]=>
  string(10) "2147483640"
}
array(1) {
  [0]=>
  string(4) "4667"
}
array(1) {
  [0]=>
  string(4) "4779"
}
array(1) {
  [0]=>
  string(4) "4095"
}
array(1) {
  [0]=>
  string(3) "250"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(10) "2147483647"
}
array(1) {
  [0]=>
  string(10) "2147483647"
}
array(1) {
  [0]=>
  string(2) "83"
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
  string(10) "2147483647"
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

