--TEST--
Test fscanf() function: usage variations - string formats with strings
--FILE--
<?php

/*
  Prototype: mixed fscanf ( resource $handle, string $format [, mixed &$...] );
  Description: Parses input from a file according to a format
*/

/* Test fscanf() to scan different strings using different string format types */

$file_path = dirname(__FILE__);

echo "*** Test fscanf(): different string format types with different string ***\n";

// create a file
$filename = "$file_path/fscanf_variation14.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// different valid strings

/* string created using Heredoc (<<<) */
$heredoc_string = <<<EOT
This is string defined
using heredoc.
EOT;
/* heredoc string with only numerics */
$heredoc_numeric_string = <<<EOT
123456 3993
4849 string
EOT;
/* null heardoc string */
$heredoc_empty_string = <<<EOT
EOT;
$heredoc_null_string = <<<EOT
NULL
EOT;

$valid_strings = array(
  "",
  " ",
  '',
  ' ',
  "string",
  'string',
  "NULL",
  'null',
  "FALSE",
  'true',
  "\x0b",
  "\0",
  '\0',
  '\060',
  "\070",
  "0x55F",
  "055",
  "@#$#$%%$^^$%^%^$^&",
  $heredoc_string,
  $heredoc_numeric_string,
  $heredoc_empty_string,
  $heredoc_null_string
);

// various string formats
$string_formats = array( "%s",
                         "%hs", "%ls", "%Ls",
                         " %s", "%s ", "% s",
                         "\t%s", "\n%s", "%4s",
                         "%30s", "%[a-zA-Z0-9]", "%*s"
                  );

$counter = 1;

// writing to the file
foreach($valid_strings as $string) {
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
// reading the values from file using different string formats
foreach($string_formats as $string_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    var_dump( fscanf($file_handle,$string_format) );
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = dirname(__FILE__);
$filename = "$file_path/fscanf_variation14.tmp";
unlink($filename);
?>
--EXPECTF--
*** Test fscanf(): different string format types with different string ***

-- iteration 1 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 2 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 3 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 4 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 5 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 6 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
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
bool(false)

-- iteration 8 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 9 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 10 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(4) "FALS"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(4) "0x55"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(4) "usin"
}
array(1) {
  [0]=>
  string(4) "1234"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 11 --
NULL
NULL
NULL
NULL
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
NULL
NULL
array(1) {
  [0]=>
  string(2) "\0"
}
array(1) {
  [0]=>
  string(4) "\060"
}
array(1) {
  [0]=>
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
NULL
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
NULL
array(1) {
  [0]=>
  string(4) "NULL"
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
  string(4) "NULL"
}
array(1) {
  [0]=>
  string(4) "null"
}
array(1) {
  [0]=>
  string(5) "FALSE"
}
array(1) {
  [0]=>
  string(4) "true"
}
array(1) {
  [0]=>
  NULL
}
NULL
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
  string(1) "8"
}
array(1) {
  [0]=>
  string(5) "0x55F"
}
array(1) {
  [0]=>
  string(3) "055"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(4) "This"
}
array(1) {
  [0]=>
  string(5) "using"
}
array(1) {
  [0]=>
  string(6) "123456"
}
array(1) {
  [0]=>
  string(4) "4849"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(4) "NULL"
}
bool(false)

-- iteration 13 --
NULL
NULL
NULL
NULL
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
NULL
NULL
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
NULL
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
NULL
array(0) {
}
bool(false)

*** Done ***
