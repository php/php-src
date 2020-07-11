--TEST--
Test ctype_cntrl() function : usage variations - Different strings
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/*
 * Pass strings containing different character types to ctype_cntrl() to test
 * which are considered valid control character only strings
 */

echo "*** Testing ctype_cntrl() : usage variations ***\n";

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
       "\n",
/*25*/ "\r",
       "\t",
       "0xD",
       "0xA",
       "0xE",
/*30*/ "\t\r\n",
);


// loop through each element of $values to test behaviour of ctype_cntrl()
$iterator = 1;
foreach($values as $value) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( ctype_cntrl($value) );
      $iterator++;
};

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_cntrl() : usage variations ***

-- Iteration 1 --
bool(false)

-- Iteration 2 --
bool(false)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
bool(true)

-- Iteration 11 --
bool(false)

-- Iteration 12 --
bool(false)

-- Iteration 13 --
bool(false)

-- Iteration 14 --
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --
bool(false)

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
bool(true)

-- Iteration 26 --
bool(true)

-- Iteration 27 --
bool(false)

-- Iteration 28 --
bool(false)

-- Iteration 29 --
bool(false)

-- Iteration 30 --
bool(true)
