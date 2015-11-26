--TEST--
Test strtok() function : usage variations - invalid escape sequences as tokens
--FILE--
<?php
/* Prototype  : string strtok ( str $str, str $token )
 * Description: splits a string (str) into smaller strings (tokens), with each token being delimited by any character from token
 * Source code: ext/standard/string.c
*/

/*
 * Testing strtok() : with invalid escape sequences in token
*/

echo "*** Testing strtok() : with invalid escape sequences in token ***\n";

// defining arrays for input strings and tokens
$string_array = array(
 		       "khellok worldk",
 		       "\khello\k world\k",
 		       "/khello\k world/k",
 		       "/hellok/ world"
 		     );
$token_array = array( 
		       "k",
		       "/ ",
		       "/k",
		       "\k",
		       "\\\\\\\k\h\\e\l\o\w\r\l\d" 
 		    );

// loop through each element of the array and check the working of strtok()
// when supplied with different string and token values

$counter =1;
foreach( $string_array as $string )  {
  echo "\n--- Iteration $counter ---\n";
  foreach( $token_array as $token )  { 
    var_dump( strtok($string, $token) ); 
    for( $count = 1; $count <=3; $count++ )  {
      var_dump( strtok($token) );
    }
    echo "\n";
  }
  $counter++;
}		      
		      

echo "Done\n";
?>
--EXPECTF--
*** Testing strtok() : with invalid escape sequences in token ***

--- Iteration 1 ---
string(5) "hello"
string(6) " world"
bool(false)
bool(false)

string(7) "khellok"
string(6) "worldk"
bool(false)
bool(false)

string(5) "hello"
string(6) " world"
bool(false)
bool(false)

string(5) "hello"
string(6) " world"
bool(false)
bool(false)

string(1) " "
string(1) "r"
bool(false)
bool(false)


--- Iteration 2 ---
string(1) "\"
string(6) "hello\"
string(7) " world\"
bool(false)

string(9) "\khello\k"
string(7) "world\k"
bool(false)
bool(false)

string(1) "\"
string(6) "hello\"
string(7) " world\"
bool(false)

string(5) "hello"
string(6) " world"
bool(false)
bool(false)

string(1) " "
string(1) "r"
bool(false)
bool(false)


--- Iteration 3 ---
string(1) "/"
string(6) "hello\"
string(7) " world/"
bool(false)

string(8) "khello\k"
string(5) "world"
string(1) "k"
bool(false)

string(6) "hello\"
string(6) " world"
bool(false)
bool(false)

string(1) "/"
string(5) "hello"
string(7) " world/"
bool(false)

string(1) "/"
string(1) " "
string(1) "r"
string(1) "/"


--- Iteration 4 ---
string(6) "/hello"
string(7) "/ world"
bool(false)
bool(false)

string(6) "hellok"
string(5) "world"
bool(false)
bool(false)

string(5) "hello"
string(6) " world"
bool(false)
bool(false)

string(6) "/hello"
string(7) "/ world"
bool(false)
bool(false)

string(1) "/"
string(2) "/ "
string(1) "r"
bool(false)

Done
