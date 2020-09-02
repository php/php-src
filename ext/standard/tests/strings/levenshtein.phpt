--TEST--
levenshtein() function test
--FILE--
<?php

echo '--- Equal ---' . \PHP_EOL;
var_dump(levenshtein('12345', '12345'));

echo '--- First string empty ---' . \PHP_EOL;
var_dump(levenshtein('', 'xyz'));
echo '--- Second string empty ---' . \PHP_EOL;
var_dump(levenshtein('xyz', ''));
echo '--- Both empty ---' . \PHP_EOL;
var_dump(levenshtein('', ''));
var_dump(levenshtein('', '', 10, 10, 10));

echo '--- 1 character ---' . \PHP_EOL;
var_dump(levenshtein('1', '2'));
echo '--- 2 character swapped ---' . \PHP_EOL;
var_dump(levenshtein('12', '21'));

echo '--- Inexpensive deletion ---' . \PHP_EOL;
var_dump(levenshtein('2121', '11', 2));
echo '--- Expensive deletion ---' . \PHP_EOL;
var_dump(levenshtein('2121', '11', 2, 1, 5));

echo '--- Inexpensive insertion ---' . \PHP_EOL;
var_dump(levenshtein('11', '2121'));
echo '--- Expensive insertion ---' . \PHP_EOL;
var_dump(levenshtein('11', '2121', 5));

echo '--- Expensive replacement ---' . \PHP_EOL;
var_dump(levenshtein('111', '121', 2, 3, 2));
echo '--- Very expensive replacement ---' . \PHP_EOL;
var_dump(levenshtein('111', '121', 2, 9, 2));

?>
--EXPECT--
--- Equal ---
int(0)
--- First string empty ---
int(3)
--- Second string empty ---
int(3)
--- Both empty ---
int(0)
int(0)
--- 1 character ---
int(1)
--- 2 character swapped ---
int(2)
--- Inexpensive deletion ---
int(2)
--- Expensive deletion ---
int(10)
--- Inexpensive insertion ---
int(2)
--- Expensive insertion ---
int(10)
--- Expensive replacement ---
int(3)
--- Very expensive replacement ---
int(4)
