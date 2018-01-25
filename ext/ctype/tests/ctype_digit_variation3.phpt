--TEST--
Test ctype_digit() function : usage variations - different strings
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_digit(mixed $c)
 * Description: Checks for numeric character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass strings containing different character types to ctype_digit() to test
 * which are considered valid decimal digit only strings
 */

echo "*** Testing ctype_digit() : usage variations ***\n";

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
       ")speci@! ch@r$(",
       '@!$*',
       "0",
/*15*/ "3",
       "9",
       "1234",
       "7890",
       "0677",
/*20*/ '0',
       '3',
       '9',
       '1234',
       '7890',
/*25*/ "123abc",
       "abc123",
       "123\r\t",
       "123 ",
       "  123",
/*30*/ "123E4",
/*31*/ "0x3F",
);

$iterator = 1;
foreach($values as $value) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( ctype_digit($value) );
      $iterator++;
};

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_digit() : usage variations ***

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
bool(false)

-- Iteration 11 --
bool(true)

-- Iteration 12 --
bool(false)

-- Iteration 13 --
bool(false)

-- Iteration 14 --
bool(true)

-- Iteration 15 --
bool(true)

-- Iteration 16 --
bool(true)

-- Iteration 17 --
bool(true)

-- Iteration 18 --
bool(true)

-- Iteration 19 --
bool(true)

-- Iteration 20 --
bool(true)

-- Iteration 21 --
bool(true)

-- Iteration 22 --
bool(true)

-- Iteration 23 --
bool(true)

-- Iteration 24 --
bool(true)

-- Iteration 25 --
bool(false)

-- Iteration 26 --
bool(false)

-- Iteration 27 --
bool(false)

-- Iteration 28 --
bool(false)

-- Iteration 29 --
bool(false)

-- Iteration 30 --
bool(false)

-- Iteration 31 --
bool(false)
===DONE===
