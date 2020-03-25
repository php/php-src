--TEST--
Test strtok() function : basic functionality
--FILE--
<?php
/* Prototype  : string strtok ( str $str, str $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() : basic functionality
*/

echo "*** Testing strtok() : basic functionality ***\n";

// Initialize all required variables
$str = 'This testcase test strtok() function.';
$token = ' ().';

echo "\nThe Input string is:\n\"$str\"\n";
echo "\nThe token string is:\n\"$token\"\n";

// using strtok() with $str argument
echo "\n--- Token 1 ---\n";
var_dump( strtok($str, $token) );

for( $i = 2; $i <=7; $i++ )  {
  echo "\n--- Token $i ---\n";
  var_dump( strtok($token) );
}

echo "Done\n";
?>
--EXPECT--
*** Testing strtok() : basic functionality ***

The Input string is:
"This testcase test strtok() function."

The token string is:
" ()."

--- Token 1 ---
string(4) "This"

--- Token 2 ---
string(8) "testcase"

--- Token 3 ---
string(4) "test"

--- Token 4 ---
string(6) "strtok"

--- Token 5 ---
string(8) "function"

--- Token 6 ---
bool(false)

--- Token 7 ---
bool(false)
Done
