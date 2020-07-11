--TEST--
Test fscanf() function: usage variations - octal formats with float values
--FILE--
<?php

/* Test fscanf() to scan float values using different octal format types */

$file_path = __DIR__;

echo "*** Test fscanf(): different octal format types with float values ***\n";

// create a file
$filename = "$file_path/fscanf_variation28.tmp";
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

$octal_formats = array( "%o",
                        "%ho", "%lo", "%Lo",
                        " %o", "%o ", "% o",
                        "\t%o", "\n%o", "%4o",
                        "%30o", "%[0-7]", "%*o"
                 );

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
// reading the values from file using different octal formats
foreach($octal_formats as $octal_format) {
  // rewind the file so that for every foreach iteration the file pointer starts from bof
  rewind($file_handle);
  echo "\n-- iteration $counter --\n";
  while( !feof($file_handle) ) {
    try {
      var_dump(fscanf($file_handle,$octal_format));
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
$filename = "$file_path/fscanf_variation28.tmp";
unlink($filename);
?>
--EXPECT--
*** Test fscanf(): different octal format types with float values ***

-- iteration 1 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 2 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 3 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 4 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 5 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 6 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
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
bool(false)

-- iteration 8 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 9 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 10 --
array(1) {
  [0]=>
  int(-140)
}
array(1) {
  [0]=>
  int(1127)
}
array(1) {
  [0]=>
  int(-140)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(1127)
}
array(1) {
  [0]=>
  int(-140)
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
  int(512)
}
array(1) {
  [0]=>
  int(-64)
}
array(1) {
  [0]=>
  int(512)
}
array(1) {
  [0]=>
  int(-64)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(552)
}
bool(false)

-- iteration 11 --
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
}
array(1) {
  [0]=>
  int(1821)
}
array(1) {
  [0]=>
  int(9020)
}
array(1) {
  [0]=>
  int(-9020)
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
  int(32768)
}
array(1) {
  [0]=>
  int(-262144)
}
array(1) {
  [0]=>
  int(16777216)
}
array(1) {
  [0]=>
  int(-134217728)
}
array(1) {
  [0]=>
  int(8)
}
array(1) {
  [0]=>
  int(282624)
}
bool(false)

-- iteration 12 --
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(5) "21474"
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  string(4) "3435"
}
array(1) {
  [0]=>
  string(5) "21474"
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
