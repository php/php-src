--TEST--
Test fscanf() function: usage variations - hexa formats with integer values
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php

/* Test fscanf() to scan different integer values using different hexa format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different hexa format types with different integer values ***\n";

// create a file
$filename = "$file_path/fscanf_variation33.tmp";
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
// various hexa formats
$hexa_formats = array(  "%x",
            "%xx", "%lx", "%Lx",
            " %x", "%x ", "% x",
                "\t%x", "\n%x", "%4x",
                "%30x", "%[0-9A-Fa-f]", "%*x"
        );

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
// reading the values from file using different hexa formats
foreach($hexa_formats as $hexa_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    try {
      var_dump(fscanf($file_handle,$hexa_format));
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
$filename = "$file_path/fscanf_variation33.tmp";
unlink($filename);
?>
--EXPECT--
*** Test fscanf(): different hexa format types with different integer values ***

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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(2147483647)
}
bool(false)

-- iteration 7 --
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
Bad scan conversion character " "
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-532)
}
array(1) {
  [0]=>
  int(-532)
}
array(1) {
  [0]=>
  int(8519)
}
array(1) {
  [0]=>
  int(8519)
}
array(1) {
  [0]=>
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-532)
}
array(1) {
  [0]=>
  int(8519)
}
array(1) {
  [0]=>
  int(8519)
}
array(1) {
  [0]=>
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-532)
}
array(1) {
  [0]=>
  int(8519)
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
  int(-1)
}
array(1) {
  [0]=>
  int(-2147483648)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(18023)
}
array(1) {
  [0]=>
  int(18297)
}
array(1) {
  [0]=>
  int(16533)
}
array(1) {
  [0]=>
  int(592)
}
array(1) {
  [0]=>
  int(-2147483648)
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
  int(131)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(-2147483648)
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
