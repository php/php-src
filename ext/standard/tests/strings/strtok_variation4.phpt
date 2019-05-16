--TEST--
Test strtok() function : usage variations - with embedded nulls in the strings
--FILE--
<?php
/* Prototype  : string strtok ( str $str, str $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() : with embedded nulls in the strings
*/

echo "*** Testing strtok() : with embedded nulls in the strings ***\n";

// defining varous strings with embedded nulls
$strings_with_nulls = array(
 		           "\0",
 		           '\0',
                           "hello\0world",
                           "\0hel\0lo",
                           "hello\0",
                           "\0\0hello\tworld\0\0",
                           "\\0he\0llo\\0",
                           'hello\0\0'
                           );

// loop through each element of the array and check the working of strtok()
// when supplied with different string values

$counter = 1;
foreach( $strings_with_nulls as $string )  {
  echo "\n--- Iteration $counter ---\n";
  var_dump( strtok($string, "\0") );
  for($count = 1; $count <= 5; $count++)  {
    var_dump( strtok("\0") );
  }
  $counter++;
}


echo "Done\n";
?>
--EXPECT--
*** Testing strtok() : with embedded nulls in the strings ***

--- Iteration 1 ---
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 2 ---
string(2) "\0"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 3 ---
string(5) "hello"
string(5) "world"
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 4 ---
string(3) "hel"
string(2) "lo"
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 5 ---
string(5) "hello"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 6 ---
string(11) "hello	world"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 7 ---
string(4) "\0he"
string(5) "llo\0"
bool(false)
bool(false)
bool(false)
bool(false)

--- Iteration 8 ---
string(9) "hello\0\0"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
