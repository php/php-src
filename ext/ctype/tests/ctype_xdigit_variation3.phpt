--TEST--
Test ctype_xdigit() function : usage variations - Different strings
--EXTENSIONS--
ctype
--FILE--
<?php
/*
 * Pass strings containing different character types to ctype_xdigit() to test
 * which are considered valid hexadecimal 'digit' only strings
 */

echo "*** Testing ctype_xdigit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

$values = array(
/*1*/  "This string contains just letters and spaces", // Simple string
       "but this one contains some numbers too 123+456 = 678", // Mixed string
       "",
       " ",
/*5*/  "a",
       "ABCXYZ",
       "abcxyz",
       "ABCXYZ123DEF456",
       "abczyz123DEF456",
/*10*/ "\r\n",
       "123",
       "03F", // hexadecimal 'digits'
       ")speci@! ch@r$(",
       '@!$*',
/*15*/ 'ABC',
       'abc',
       'ABC123',
       'abc123',
       'abc123\n',
/*20*/ 'abc 123',
       '',
       ' ',
       base64_decode("w4DDoMOHw6fDiMOo"), // non-ascii characters
       'ABCdef07',
       "56ea\tFB",
       "0x2A"
       );

$iterator = 1;
foreach($values as $value) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( ctype_xdigit($value) );
      $iterator++;
};

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_xdigit() : usage variations ***

-- Iteration 1 --
bool(false)

-- Iteration 2 --
bool(false)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --
bool(true)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
bool(false)

-- Iteration 11 --
bool(true)

-- Iteration 12 --
bool(true)

-- Iteration 13 --
bool(false)

-- Iteration 14 --
bool(false)

-- Iteration 15 --
bool(true)

-- Iteration 16 --
bool(true)

-- Iteration 17 --
bool(true)

-- Iteration 18 --
bool(true)

-- Iteration 19 --
bool(false)

-- Iteration 20 --
bool(false)

-- Iteration 21 --
bool(false)

-- Iteration 22 --
bool(false)

-- Iteration 23 --
bool(false)

-- Iteration 24 --
bool(true)

-- Iteration 25 --
bool(false)

-- Iteration 26 --
bool(false)
