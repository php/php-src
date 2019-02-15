--TEST--
Test stripos() function : usage variations - repetitive chars for 'haystack' argument
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function with strings containing repetitive chars for haystak
 *  and with various needles & offsets
*/

echo "*** Testing stripos() function: strings repetitive chars ***\n";
$haystack = "aBAbaBAbaBabAbAbaBa";
$needles = array(
  "aba",
  "aBA",
  "ABA",
  "Aba",
  "BAb",
  "bab",
  "bAb",
  "BAB"
);

/* loop through to consider various offsets in getting the position of the needle in haystack string */
$count = 1;
for($index = 0; $index < count($needles); $index++) {
  echo "\n-- Iteration $count --\n";
  for($offset = 0; $offset <= strlen($haystack); $offset++ ) {
    var_dump( stripos($haystack, $needles[$index], $offset) );
  }
  $count++;
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing stripos() function: strings repetitive chars ***

-- Iteration 1 --
int(0)
int(2)
int(2)
int(4)
int(4)
int(6)
int(6)
int(8)
int(8)
int(10)
int(10)
int(12)
int(12)
int(14)
int(14)
int(16)
int(16)
bool(false)
bool(false)
bool(false)

-- Iteration 2 --
int(0)
int(2)
int(2)
int(4)
int(4)
int(6)
int(6)
int(8)
int(8)
int(10)
int(10)
int(12)
int(12)
int(14)
int(14)
int(16)
int(16)
bool(false)
bool(false)
bool(false)

-- Iteration 3 --
int(0)
int(2)
int(2)
int(4)
int(4)
int(6)
int(6)
int(8)
int(8)
int(10)
int(10)
int(12)
int(12)
int(14)
int(14)
int(16)
int(16)
bool(false)
bool(false)
bool(false)

-- Iteration 4 --
int(0)
int(2)
int(2)
int(4)
int(4)
int(6)
int(6)
int(8)
int(8)
int(10)
int(10)
int(12)
int(12)
int(14)
int(14)
int(16)
int(16)
bool(false)
bool(false)
bool(false)

-- Iteration 5 --
int(1)
int(1)
int(3)
int(3)
int(5)
int(5)
int(7)
int(7)
int(9)
int(9)
int(11)
int(11)
int(13)
int(13)
int(15)
int(15)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration 6 --
int(1)
int(1)
int(3)
int(3)
int(5)
int(5)
int(7)
int(7)
int(9)
int(9)
int(11)
int(11)
int(13)
int(13)
int(15)
int(15)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration 7 --
int(1)
int(1)
int(3)
int(3)
int(5)
int(5)
int(7)
int(7)
int(9)
int(9)
int(11)
int(11)
int(13)
int(13)
int(15)
int(15)
bool(false)
bool(false)
bool(false)
bool(false)

-- Iteration 8 --
int(1)
int(1)
int(3)
int(3)
int(5)
int(5)
int(7)
int(7)
int(9)
int(9)
int(11)
int(11)
int(13)
int(13)
int(15)
int(15)
bool(false)
bool(false)
bool(false)
bool(false)
*** Done ***
