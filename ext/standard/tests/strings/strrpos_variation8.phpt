--TEST--
Test strrpos() function : usage variations - repetitive chars for 'haystack' argument
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

/* Test strrpos() function with strings containing multiple occurrences of 'needle' in the 'haystack'
 *  and with various needles & offsets
*/

echo "*** Testing strrpos() function: strings repetitive chars ***\n";
$haystack = "ababababAbaBa";
$needle = "aba";

/* loop through to consider various offsets in getting the position of the needle in haystack string */
$count = 1;
for($offset = -1; $offset <= strlen($haystack); $offset++ ) {
  echo "-- Iteration $count --\n";
  var_dump( strrpos($haystack, $needle, $offset) );
  $count++;
}
echo "*** Done ***";
?>
--EXPECT--
*** Testing strrpos() function: strings repetitive chars ***
-- Iteration 1 --
int(4)
-- Iteration 2 --
int(4)
-- Iteration 3 --
int(4)
-- Iteration 4 --
int(4)
-- Iteration 5 --
int(4)
-- Iteration 6 --
int(4)
-- Iteration 7 --
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(false)
-- Iteration 10 --
bool(false)
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
*** Done ***
