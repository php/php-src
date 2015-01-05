--TEST--
Test chunk_split() function : usage variations - different integer values for 'chunklen' with heredoc string as 'str'(Bug#42796)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: 
*/

/*
* passing different integer values for 'chunklen' and heredoc string for 'str' to chunk_split()
*/

echo "*** Testing chunk_split() : different 'chunklen' with heredoc 'str' ***\n";


// Initializing required variables
//heredoc string as str
$heredoc_str = <<<EOT
This's heredoc string with \t and \n white space char.
It has _speci@l ch@r$ 2222 !!!Now \k as escape char to test
chunk_split()
EOT;

$ending = ':::';

// different values for 'chunklen'
$values = array (
  0,  
  1,  
  -123,  //negative integer
  0234,  //octal number
  0x1A,  //hexadecimal number
  PHP_INT_MAX,      // max positive integer number
  PHP_INT_MAX * 3,  // integer overflow
  -PHP_INT_MAX - 1, // min negative integer

);


// loop through each element of values for 'chunklen'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1). " --\n";
  var_dump( chunk_split($heredoc_str, $values[$count], $ending) );
}

echo "Done"
?>
--EXPECTF--
*** Testing chunk_split() : different 'chunklen' with heredoc 'str' ***
-- Iteration 1 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 2 --
string(504) "T:::h:::i:::s:::':::s::: :::h:::e:::r:::e:::d:::o:::c::: :::s:::t:::r:::i:::n:::g::: :::w:::i:::t:::h::: :::	::: :::a:::n:::d::: :::
::: :::w:::h:::i:::t:::e::: :::s:::p:::a:::c:::e::: :::c:::h:::a:::r:::.:::
:::I:::t::: :::h:::a:::s::: :::_:::s:::p:::e:::c:::i:::@:::l::: :::c:::h:::@:::r:::$::: :::2:::2:::2:::2::: :::!:::!:::!:::N:::o:::w::: :::\:::k::: :::a:::s::: :::e:::s:::c:::a:::p:::e::: :::c:::h:::a:::r::: :::t:::o::: :::t:::e:::s:::t:::
:::c:::h:::u:::n:::k:::_:::s:::p:::l:::i:::t:::(:::):::"
-- Iteration 3 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
-- Iteration 4 --
string(129) "This's heredoc string with 	 and 
 white space char.
It has _speci@l ch@r$ 2222 !!!Now \k as escape char to test
chunk_split():::"
-- Iteration 5 --
string(141) "This's heredoc string with::: 	 and 
 white space char.:::
It has _speci@l ch@r$ 222:::2 !!!Now \k as escape char::: to test
chunk_split():::"
-- Iteration 6 --
string(129) "This's heredoc string with 	 and 
 white space char.
It has _speci@l ch@r$ 2222 !!!Now \k as escape char to test
chunk_split():::"
-- Iteration 7 --

Warning: chunk_split() expects parameter 2 to be integer, float given in %s on line %d
NULL
-- Iteration 8 --

Warning: chunk_split(): Chunk length should be greater than zero in %s on line %d
bool(false)
Done
