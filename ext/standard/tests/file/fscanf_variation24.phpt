--TEST--
Test fscanf() function: usage variations - char formats with strings
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan strings using different char format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different char format types with strings ***\n"; 

// create a file
$filename = "$file_path/fscanf_variation24.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array of strings
$strings = array (
  "",
  '',
  "0",
  '0',
  "1",
  '1',
  "\x01",
  '\x01',
  "\01",
  '\01',
  'string',
  "string",
  "true",
  "FALSE",
  'false',
  'TRUE',
  "NULL",
  'null'
);

$char_formats = array( "%c",
		       "%hc", "%lc", "%Lc", 
		       " %c", "%c ", "% c",
		       "\t%c", "\n%c", "%4c", 
		       "%30c", "%[a-zA-Z@#$&0-9]", "%*c");

$counter = 1;

// writing to the file
foreach($strings as $string) {
  @fprintf($file_handle, $string);
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
$filename = "$file_path/fscanf_variation24.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different char format types with strings ***

-- iteration 1 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 5 --
NULL
NULL
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
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
NULL
NULL
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 9 --
NULL
NULL
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(1) "\"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "F"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "T"
}
array(1) {
  [0]=>
  string(1) "N"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(4) "\x01"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(3) "\01"
}
array(1) {
  [0]=>
  string(4) "stri"
}
array(1) {
  [0]=>
  string(4) "stri"
}
array(1) {
  [0]=>
  string(4) "true"
}
array(1) {
  [0]=>
  string(4) "FALS"
}
array(1) {
  [0]=>
  string(4) "fals"
}
array(1) {
  [0]=>
  string(4) "TRUE"
}
array(1) {
  [0]=>
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "0"
}
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
  string(1) "1"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(4) "\x01"
}
array(1) {
  [0]=>
  string(1) ""
}
array(1) {
  [0]=>
  string(3) "\01"
}
array(1) {
  [0]=>
  string(6) "string"
}
array(1) {
  [0]=>
  string(6) "string"
}
array(1) {
  [0]=>
  string(4) "true"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(5) "false"
}
array(1) {
  [0]=>
  string(4) "TRUE"
}
array(1) {
  [0]=>
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
bool(false)

-- iteration 12 --
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
  string(1) "0"
}
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
  string(6) "string"
}
array(1) {
  [0]=>
  string(6) "string"
}
array(1) {
  [0]=>
  string(4) "true"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(5) "false"
}
array(1) {
  [0]=>
  string(4) "TRUE"
}
array(1) {
  [0]=>
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
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

