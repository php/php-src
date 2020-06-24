--TEST--
Test image_type_to_mime_type() function : usage variations  - Pass decimal, octal, and hexadecimal values as imagetype
--FILE--
<?php
echo "*** Testing image_type_to_mime_type() : usage variations ***\n";

error_reporting(E_ALL ^ E_NOTICE);
$values =  array (
  //Decimal values
  0,
  1,
  12345,
  -12345,

  //Octal values
  02,
  010,
  030071,
  -030071,

  //Hexadecimal values
  0x0,
  0x1,
  0xABCD,
  -0xABCD
);

// loop through each element of the array for imagetype
$iterator = 1;
foreach($values as $value) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( image_type_to_mime_type($value) );
      $iterator++;
};
?>
--EXPECT--
*** Testing image_type_to_mime_type() : usage variations ***

-- Iteration 1 --
string(24) "application/octet-stream"

-- Iteration 2 --
string(9) "image/gif"

-- Iteration 3 --
string(24) "application/octet-stream"

-- Iteration 4 --
string(24) "application/octet-stream"

-- Iteration 5 --
string(10) "image/jpeg"

-- Iteration 6 --
string(10) "image/tiff"

-- Iteration 7 --
string(24) "application/octet-stream"

-- Iteration 8 --
string(24) "application/octet-stream"

-- Iteration 9 --
string(24) "application/octet-stream"

-- Iteration 10 --
string(9) "image/gif"

-- Iteration 11 --
string(24) "application/octet-stream"

-- Iteration 12 --
string(24) "application/octet-stream"
