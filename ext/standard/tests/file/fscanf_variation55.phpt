--TEST--
Test fscanf() function: usage variations - tracking file pointer while reading
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php

/*
  Test fscanf() to scan data using different format types and also
  tracking the file pointer movement along with reading
 */

$file_path = __DIR__;

echo "*** Test fscanf(): tracking file pointer along with reading data from file ***\n";

// create a file
$filename = "$file_path/fscanf_variation55.tmp";
$file_handle = fopen($filename, "w");
if($file_handle == false)
  exit("Error:failed to open file $filename");

// different valid data
$valid_data = array(
  12345,            // integer value
  -12345,
  123.45,	    // float value
  -123.45,
  0x123B,           // hexadecimal value
  0x12ab,
  0123,             // octal value
  -0123,
  "abcde",          // string
  'abcde',
  10e3,             // exponential value
  10e-3
);
// various formats
$int_formats = array( "%d", "%f", "%s", "%o", "%x", "%u", "%c", "%e");

$counter = 1;

// writing to the file
foreach($valid_data as $data) {
  @fprintf($file_handle, $data);
  @fprintf($file_handle, "\n");
}
// closing the file
fclose($file_handle);

$modes = array("r", "rb", "rt");

foreach($modes as $mode) {

  echo "\n*** File opened in $mode mode ***\n";
  // opening the file for reading
  $file_handle = fopen($filename, $mode);
  if($file_handle == false) {
    exit("Error:failed to open file $filename");
  }

  $counter = 1;
  // reading different data from file using different formats
  foreach($int_formats as $int_format) {
    // current file pointer position
    var_dump( ftell($file_handle) );
    // rewind the file so that for every foreach iteration the file pointer starts from bof
    rewind($file_handle);
    // current file pointer position after rewind operation
    var_dump( ftell($file_handle) );
    echo "\n-- iteration $counter --\n";
    while( !feof($file_handle) ) {
      var_dump( ftell($file_handle) );
      var_dump( fscanf($file_handle,$int_format) );
    }
    $counter++;
  } // end of inner for loop
} // end of outer for loop

echo "\n*** Done ***";
?>
--CLEAN--
<?php
$file_path = __DIR__;
$filename = "$file_path/fscanf_variation55.tmp";
unlink($filename);
?>
--EXPECT--
*** Test fscanf(): tracking file pointer along with reading data from file ***

*** File opened in r mode ***
int(0)
int(0)

-- iteration 1 --
int(0)
array(1) {
  [0]=>
  int(12345)
}
int(6)
array(1) {
  [0]=>
  int(-12345)
}
int(13)
array(1) {
  [0]=>
  int(123)
}
int(20)
array(1) {
  [0]=>
  int(-123)
}
int(28)
array(1) {
  [0]=>
  int(4667)
}
int(33)
array(1) {
  [0]=>
  int(4779)
}
int(38)
array(1) {
  [0]=>
  int(83)
}
int(41)
array(1) {
  [0]=>
  int(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(10000)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 2 --
int(0)
array(1) {
  [0]=>
  float(12345)
}
int(6)
array(1) {
  [0]=>
  float(-12345)
}
int(13)
array(1) {
  [0]=>
  float(123.45)
}
int(20)
array(1) {
  [0]=>
  float(-123.45)
}
int(28)
array(1) {
  [0]=>
  float(4667)
}
int(33)
array(1) {
  [0]=>
  float(4779)
}
int(38)
array(1) {
  [0]=>
  float(83)
}
int(41)
array(1) {
  [0]=>
  float(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  float(10000)
}
int(63)
array(1) {
  [0]=>
  float(0.01)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 3 --
int(0)
array(1) {
  [0]=>
  string(5) "12345"
}
int(6)
array(1) {
  [0]=>
  string(6) "-12345"
}
int(13)
array(1) {
  [0]=>
  string(6) "123.45"
}
int(20)
array(1) {
  [0]=>
  string(7) "-123.45"
}
int(28)
array(1) {
  [0]=>
  string(4) "4667"
}
int(33)
array(1) {
  [0]=>
  string(4) "4779"
}
int(38)
array(1) {
  [0]=>
  string(2) "83"
}
int(41)
array(1) {
  [0]=>
  string(3) "-83"
}
int(45)
array(1) {
  [0]=>
  string(5) "abcde"
}
int(51)
array(1) {
  [0]=>
  string(5) "abcde"
}
int(57)
array(1) {
  [0]=>
  string(5) "10000"
}
int(63)
array(1) {
  [0]=>
  string(4) "0.01"
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 4 --
int(0)
array(1) {
  [0]=>
  int(5349)
}
int(6)
array(1) {
  [0]=>
  int(-5349)
}
int(13)
array(1) {
  [0]=>
  int(83)
}
int(20)
array(1) {
  [0]=>
  int(-83)
}
int(28)
array(1) {
  [0]=>
  int(2487)
}
int(33)
array(1) {
  [0]=>
  int(319)
}
int(38)
array(1) {
  [0]=>
  NULL
}
int(41)
array(1) {
  [0]=>
  NULL
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(4096)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 5 --
int(0)
array(1) {
  [0]=>
  int(74565)
}
int(6)
array(1) {
  [0]=>
  int(-74565)
}
int(13)
array(1) {
  [0]=>
  int(291)
}
int(20)
array(1) {
  [0]=>
  int(-291)
}
int(28)
array(1) {
  [0]=>
  int(18023)
}
int(33)
array(1) {
  [0]=>
  int(18297)
}
int(38)
array(1) {
  [0]=>
  int(131)
}
int(41)
array(1) {
  [0]=>
  int(-131)
}
int(45)
array(1) {
  [0]=>
  int(703710)
}
int(51)
array(1) {
  [0]=>
  int(703710)
}
int(57)
array(1) {
  [0]=>
  int(65536)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 6 --
int(0)
array(1) {
  [0]=>
  int(12345)
}
int(6)
array(1) {
  [0]=>
  string(10) "4294954951"
}
int(13)
array(1) {
  [0]=>
  int(123)
}
int(20)
array(1) {
  [0]=>
  string(10) "4294967173"
}
int(28)
array(1) {
  [0]=>
  int(4667)
}
int(33)
array(1) {
  [0]=>
  int(4779)
}
int(38)
array(1) {
  [0]=>
  int(83)
}
int(41)
array(1) {
  [0]=>
  string(10) "4294967213"
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(10000)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 7 --
int(0)
array(1) {
  [0]=>
  string(1) "1"
}
int(6)
array(1) {
  [0]=>
  string(1) "-"
}
int(13)
array(1) {
  [0]=>
  string(1) "1"
}
int(20)
array(1) {
  [0]=>
  string(1) "-"
}
int(28)
array(1) {
  [0]=>
  string(1) "4"
}
int(33)
array(1) {
  [0]=>
  string(1) "4"
}
int(38)
array(1) {
  [0]=>
  string(1) "8"
}
int(41)
array(1) {
  [0]=>
  string(1) "-"
}
int(45)
array(1) {
  [0]=>
  string(1) "a"
}
int(51)
array(1) {
  [0]=>
  string(1) "a"
}
int(57)
array(1) {
  [0]=>
  string(1) "1"
}
int(63)
array(1) {
  [0]=>
  string(1) "0"
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 8 --
int(0)
array(1) {
  [0]=>
  float(12345)
}
int(6)
array(1) {
  [0]=>
  float(-12345)
}
int(13)
array(1) {
  [0]=>
  float(123.45)
}
int(20)
array(1) {
  [0]=>
  float(-123.45)
}
int(28)
array(1) {
  [0]=>
  float(4667)
}
int(33)
array(1) {
  [0]=>
  float(4779)
}
int(38)
array(1) {
  [0]=>
  float(83)
}
int(41)
array(1) {
  [0]=>
  float(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  float(10000)
}
int(63)
array(1) {
  [0]=>
  float(0.01)
}
int(68)
bool(false)

*** File opened in rb mode ***
int(0)
int(0)

-- iteration 1 --
int(0)
array(1) {
  [0]=>
  int(12345)
}
int(6)
array(1) {
  [0]=>
  int(-12345)
}
int(13)
array(1) {
  [0]=>
  int(123)
}
int(20)
array(1) {
  [0]=>
  int(-123)
}
int(28)
array(1) {
  [0]=>
  int(4667)
}
int(33)
array(1) {
  [0]=>
  int(4779)
}
int(38)
array(1) {
  [0]=>
  int(83)
}
int(41)
array(1) {
  [0]=>
  int(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(10000)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 2 --
int(0)
array(1) {
  [0]=>
  float(12345)
}
int(6)
array(1) {
  [0]=>
  float(-12345)
}
int(13)
array(1) {
  [0]=>
  float(123.45)
}
int(20)
array(1) {
  [0]=>
  float(-123.45)
}
int(28)
array(1) {
  [0]=>
  float(4667)
}
int(33)
array(1) {
  [0]=>
  float(4779)
}
int(38)
array(1) {
  [0]=>
  float(83)
}
int(41)
array(1) {
  [0]=>
  float(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  float(10000)
}
int(63)
array(1) {
  [0]=>
  float(0.01)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 3 --
int(0)
array(1) {
  [0]=>
  string(5) "12345"
}
int(6)
array(1) {
  [0]=>
  string(6) "-12345"
}
int(13)
array(1) {
  [0]=>
  string(6) "123.45"
}
int(20)
array(1) {
  [0]=>
  string(7) "-123.45"
}
int(28)
array(1) {
  [0]=>
  string(4) "4667"
}
int(33)
array(1) {
  [0]=>
  string(4) "4779"
}
int(38)
array(1) {
  [0]=>
  string(2) "83"
}
int(41)
array(1) {
  [0]=>
  string(3) "-83"
}
int(45)
array(1) {
  [0]=>
  string(5) "abcde"
}
int(51)
array(1) {
  [0]=>
  string(5) "abcde"
}
int(57)
array(1) {
  [0]=>
  string(5) "10000"
}
int(63)
array(1) {
  [0]=>
  string(4) "0.01"
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 4 --
int(0)
array(1) {
  [0]=>
  int(5349)
}
int(6)
array(1) {
  [0]=>
  int(-5349)
}
int(13)
array(1) {
  [0]=>
  int(83)
}
int(20)
array(1) {
  [0]=>
  int(-83)
}
int(28)
array(1) {
  [0]=>
  int(2487)
}
int(33)
array(1) {
  [0]=>
  int(319)
}
int(38)
array(1) {
  [0]=>
  NULL
}
int(41)
array(1) {
  [0]=>
  NULL
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(4096)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 5 --
int(0)
array(1) {
  [0]=>
  int(74565)
}
int(6)
array(1) {
  [0]=>
  int(-74565)
}
int(13)
array(1) {
  [0]=>
  int(291)
}
int(20)
array(1) {
  [0]=>
  int(-291)
}
int(28)
array(1) {
  [0]=>
  int(18023)
}
int(33)
array(1) {
  [0]=>
  int(18297)
}
int(38)
array(1) {
  [0]=>
  int(131)
}
int(41)
array(1) {
  [0]=>
  int(-131)
}
int(45)
array(1) {
  [0]=>
  int(703710)
}
int(51)
array(1) {
  [0]=>
  int(703710)
}
int(57)
array(1) {
  [0]=>
  int(65536)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 6 --
int(0)
array(1) {
  [0]=>
  int(12345)
}
int(6)
array(1) {
  [0]=>
  string(10) "4294954951"
}
int(13)
array(1) {
  [0]=>
  int(123)
}
int(20)
array(1) {
  [0]=>
  string(10) "4294967173"
}
int(28)
array(1) {
  [0]=>
  int(4667)
}
int(33)
array(1) {
  [0]=>
  int(4779)
}
int(38)
array(1) {
  [0]=>
  int(83)
}
int(41)
array(1) {
  [0]=>
  string(10) "4294967213"
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(10000)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 7 --
int(0)
array(1) {
  [0]=>
  string(1) "1"
}
int(6)
array(1) {
  [0]=>
  string(1) "-"
}
int(13)
array(1) {
  [0]=>
  string(1) "1"
}
int(20)
array(1) {
  [0]=>
  string(1) "-"
}
int(28)
array(1) {
  [0]=>
  string(1) "4"
}
int(33)
array(1) {
  [0]=>
  string(1) "4"
}
int(38)
array(1) {
  [0]=>
  string(1) "8"
}
int(41)
array(1) {
  [0]=>
  string(1) "-"
}
int(45)
array(1) {
  [0]=>
  string(1) "a"
}
int(51)
array(1) {
  [0]=>
  string(1) "a"
}
int(57)
array(1) {
  [0]=>
  string(1) "1"
}
int(63)
array(1) {
  [0]=>
  string(1) "0"
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 8 --
int(0)
array(1) {
  [0]=>
  float(12345)
}
int(6)
array(1) {
  [0]=>
  float(-12345)
}
int(13)
array(1) {
  [0]=>
  float(123.45)
}
int(20)
array(1) {
  [0]=>
  float(-123.45)
}
int(28)
array(1) {
  [0]=>
  float(4667)
}
int(33)
array(1) {
  [0]=>
  float(4779)
}
int(38)
array(1) {
  [0]=>
  float(83)
}
int(41)
array(1) {
  [0]=>
  float(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  float(10000)
}
int(63)
array(1) {
  [0]=>
  float(0.01)
}
int(68)
bool(false)

*** File opened in rt mode ***
int(0)
int(0)

-- iteration 1 --
int(0)
array(1) {
  [0]=>
  int(12345)
}
int(6)
array(1) {
  [0]=>
  int(-12345)
}
int(13)
array(1) {
  [0]=>
  int(123)
}
int(20)
array(1) {
  [0]=>
  int(-123)
}
int(28)
array(1) {
  [0]=>
  int(4667)
}
int(33)
array(1) {
  [0]=>
  int(4779)
}
int(38)
array(1) {
  [0]=>
  int(83)
}
int(41)
array(1) {
  [0]=>
  int(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(10000)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 2 --
int(0)
array(1) {
  [0]=>
  float(12345)
}
int(6)
array(1) {
  [0]=>
  float(-12345)
}
int(13)
array(1) {
  [0]=>
  float(123.45)
}
int(20)
array(1) {
  [0]=>
  float(-123.45)
}
int(28)
array(1) {
  [0]=>
  float(4667)
}
int(33)
array(1) {
  [0]=>
  float(4779)
}
int(38)
array(1) {
  [0]=>
  float(83)
}
int(41)
array(1) {
  [0]=>
  float(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  float(10000)
}
int(63)
array(1) {
  [0]=>
  float(0.01)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 3 --
int(0)
array(1) {
  [0]=>
  string(5) "12345"
}
int(6)
array(1) {
  [0]=>
  string(6) "-12345"
}
int(13)
array(1) {
  [0]=>
  string(6) "123.45"
}
int(20)
array(1) {
  [0]=>
  string(7) "-123.45"
}
int(28)
array(1) {
  [0]=>
  string(4) "4667"
}
int(33)
array(1) {
  [0]=>
  string(4) "4779"
}
int(38)
array(1) {
  [0]=>
  string(2) "83"
}
int(41)
array(1) {
  [0]=>
  string(3) "-83"
}
int(45)
array(1) {
  [0]=>
  string(5) "abcde"
}
int(51)
array(1) {
  [0]=>
  string(5) "abcde"
}
int(57)
array(1) {
  [0]=>
  string(5) "10000"
}
int(63)
array(1) {
  [0]=>
  string(4) "0.01"
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 4 --
int(0)
array(1) {
  [0]=>
  int(5349)
}
int(6)
array(1) {
  [0]=>
  int(-5349)
}
int(13)
array(1) {
  [0]=>
  int(83)
}
int(20)
array(1) {
  [0]=>
  int(-83)
}
int(28)
array(1) {
  [0]=>
  int(2487)
}
int(33)
array(1) {
  [0]=>
  int(319)
}
int(38)
array(1) {
  [0]=>
  NULL
}
int(41)
array(1) {
  [0]=>
  NULL
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(4096)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 5 --
int(0)
array(1) {
  [0]=>
  int(74565)
}
int(6)
array(1) {
  [0]=>
  int(-74565)
}
int(13)
array(1) {
  [0]=>
  int(291)
}
int(20)
array(1) {
  [0]=>
  int(-291)
}
int(28)
array(1) {
  [0]=>
  int(18023)
}
int(33)
array(1) {
  [0]=>
  int(18297)
}
int(38)
array(1) {
  [0]=>
  int(131)
}
int(41)
array(1) {
  [0]=>
  int(-131)
}
int(45)
array(1) {
  [0]=>
  int(703710)
}
int(51)
array(1) {
  [0]=>
  int(703710)
}
int(57)
array(1) {
  [0]=>
  int(65536)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 6 --
int(0)
array(1) {
  [0]=>
  int(12345)
}
int(6)
array(1) {
  [0]=>
  string(10) "4294954951"
}
int(13)
array(1) {
  [0]=>
  int(123)
}
int(20)
array(1) {
  [0]=>
  string(10) "4294967173"
}
int(28)
array(1) {
  [0]=>
  int(4667)
}
int(33)
array(1) {
  [0]=>
  int(4779)
}
int(38)
array(1) {
  [0]=>
  int(83)
}
int(41)
array(1) {
  [0]=>
  string(10) "4294967213"
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  int(10000)
}
int(63)
array(1) {
  [0]=>
  int(0)
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 7 --
int(0)
array(1) {
  [0]=>
  string(1) "1"
}
int(6)
array(1) {
  [0]=>
  string(1) "-"
}
int(13)
array(1) {
  [0]=>
  string(1) "1"
}
int(20)
array(1) {
  [0]=>
  string(1) "-"
}
int(28)
array(1) {
  [0]=>
  string(1) "4"
}
int(33)
array(1) {
  [0]=>
  string(1) "4"
}
int(38)
array(1) {
  [0]=>
  string(1) "8"
}
int(41)
array(1) {
  [0]=>
  string(1) "-"
}
int(45)
array(1) {
  [0]=>
  string(1) "a"
}
int(51)
array(1) {
  [0]=>
  string(1) "a"
}
int(57)
array(1) {
  [0]=>
  string(1) "1"
}
int(63)
array(1) {
  [0]=>
  string(1) "0"
}
int(68)
bool(false)
int(68)
int(0)

-- iteration 8 --
int(0)
array(1) {
  [0]=>
  float(12345)
}
int(6)
array(1) {
  [0]=>
  float(-12345)
}
int(13)
array(1) {
  [0]=>
  float(123.45)
}
int(20)
array(1) {
  [0]=>
  float(-123.45)
}
int(28)
array(1) {
  [0]=>
  float(4667)
}
int(33)
array(1) {
  [0]=>
  float(4779)
}
int(38)
array(1) {
  [0]=>
  float(83)
}
int(41)
array(1) {
  [0]=>
  float(-83)
}
int(45)
array(1) {
  [0]=>
  NULL
}
int(51)
array(1) {
  [0]=>
  NULL
}
int(57)
array(1) {
  [0]=>
  float(10000)
}
int(63)
array(1) {
  [0]=>
  float(0.01)
}
int(68)
bool(false)

*** Done ***
