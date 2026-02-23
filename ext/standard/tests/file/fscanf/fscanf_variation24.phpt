--TEST--
Test fscanf() function: usage variations - char formats with strings
--FILE--
<?php

/* Test fscanf() to scan strings using different char format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different char format types with strings ***\n";

// create a file
$filename = "$file_path/fscanf_variation24.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// array of strings
$strings = [
  '',
  '0',
  '1',
  "\01",
  '\01',
  'string',
  'true',
  'false',
  'null'
];

$char_formats = array( "%c",
               "%hc", "%lc", "%Lc",
               " %c", "%c ",
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
    try {
      var_dump(fscanf($file_handle,$char_format));
    } catch (ValueError $exception) {
      echo $exception->getMessage() . "\n";
    }
  }
  $counter++;
}

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fscanf_variation24.tmp";
unlink($filename);
?>
--EXPECT--
*** Test fscanf(): different char format types with strings ***

-- iteration 1 --
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
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
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
  string(1) "0"
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
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
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
  string(1) "0"
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
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
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
  string(1) "0"
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
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 5 --
NULL
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
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
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
  string(1) "0"
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
  string(1) "s"
}
array(1) {
  [0]=>
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 7 --
NULL
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
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 8 --
NULL
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
  string(1) "t"
}
array(1) {
  [0]=>
  string(1) "f"
}
array(1) {
  [0]=>
  string(1) "n"
}
bool(false)

-- iteration 9 --
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
  string(1) "1"
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
  string(4) "true"
}
array(1) {
  [0]=>
  string(4) "fals"
}
array(1) {
  [0]=>
  string(4) "null"
}
bool(false)

-- iteration 10 --
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
  string(1) "1"
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
  string(4) "true"
}
array(1) {
  [0]=>
  string(5) "false"
}
array(1) {
  [0]=>
  string(4) "null"
}
bool(false)

-- iteration 11 --
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
  string(6) "string"
}
array(1) {
  [0]=>
  string(4) "true"
}
array(1) {
  [0]=>
  string(5) "false"
}
array(1) {
  [0]=>
  string(4) "null"
}
bool(false)

-- iteration 12 --
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
