--TEST--
Test fscanf() function: usage variations - char formats with float values
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan float values using different char format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different char format types with float values ***\n"; 

// create a file
$filename = "$file_path/fscanf_variation21.tmp";
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

$char_formats = array( "%c",
                       "%hc", "%lc", "%Lc",
		       " %c", "%c ", "% c",
 	 	       "\t%c", "\n%c", "%4c",
 		       "%30c", "%[a-zA-Z@#$&0-9]", "%*c");

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
// reading the values from file using different char formats
foreach($char_formats as $char_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$char_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_variation21.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different char format types with float values ***

-- iteration 1 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
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
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "3"
}
array(1) {
  [0]=>
  string(1) "2"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "-"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) "1"
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  string(4) "-214"
}
array(1) {
  [0]=>
  string(4) "2147"
}
array(1) {
  [0]=>
  string(4) "-214"
}
array(1) {
  [0]=>
  string(4) "3435"
}
array(1) {
  [0]=>
  string(4) "2147"
}
array(1) {
  [0]=>
  string(4) "-214"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(4) "-0.1"
}
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [0]=>
  string(4) "1000"
}
array(1) {
  [0]=>
  string(4) "-100"
}
array(1) {
  [0]=>
  string(4) "1000"
}
array(1) {
  [0]=>
  string(4) "-100"
}
array(1) {
  [0]=>
  string(2) "10"
}
array(1) {
  [0]=>
  string(4) "1050"
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  string(11) "-2147483649"
}
array(1) {
  [0]=>
  string(10) "2147483648"
}
array(1) {
  [0]=>
  string(11) "-2147483649"
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
  string(11) "-2147483649"
}
array(1) {
  [0]=>
  string(1) "0"
}
array(1) {
  [0]=>
  string(4) "-0.1"
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
  string(8) "-1000000"
}
array(1) {
  [0]=>
  string(9) "100000000"
}
array(1) {
  [0]=>
  string(11) "-1000000000"
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

