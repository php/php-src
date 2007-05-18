--TEST--
Test str_replace() function
--INI--
precision=14
--FILE--
<?php
/* 
  Prototype: mixed str_replace(mixed $search, mixed $replace, 
                               mixed $subject [, int &$count]);
  Description: Replace all occurrences of the search string with 
               the replacement string
*/

echo "\n*** Testing str_replace() on basic operations ***\n";

var_dump( str_replace("", "", "") );

var_dump( str_replace("e", "b", "test") );

var_dump( str_replace("", "", "", $count) );
var_dump( $count );

var_dump( str_replace("q", "q", "q", $count) );
var_dump( $count );

var_dump( str_replace("long string here", "", "", $count) );
var_dump( $count );

$fp = fopen( __FILE__, "r" );
var_dump( str_replace($fp, $fp, $fp, $fp) );
var_dump( $fp );

echo "\n*** Testing str_replace() with various search values ***";
$search_arr = array( TRUE, FALSE, 1, 0, -1, "1", "0", "-1",  NULL, 
                     array(), "php", "");

$i = 0;
/* loop through to replace the matched elements in the array */
foreach( $search_arr as $value ) {
  echo "\n-- Iteration $i --\n";
  /* replace the string in array */
  var_dump( str_replace($value, "FOUND", $search_arr, $count) ); 
  var_dump( $count );
  $i++;
}

echo "\n*** Testing str_replace() with various subjects ***";
$subject = "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE\000
 	    \x000\xABCD\0abcd \xXYZ\tabcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)";

/* needles in an array to be compared in the string $string */
$search_str = array ( 
  "Hello, World",
  'Hello, World',
  '!!Hello, World',
  "??Hello, World",
  "$@#%^&*!~,.:;?",
  "123",
  123,
  "-1.2345",
  -1.2344,
  "abcd",
  'XYZ',
  NULL,
  "NULL",
  "0",
  0,
  "",
  " ",
  "\0",
  "\x000",
  "\xABC",
  "\0000",
  ".3",
  TRUE,
  "TRUE",
  "1",
  1,
  FALSE,
  "FALSE",
  " ",
  "          ",
  'b',
  '\t',
  "\t",
  chr(128).chr(234).chr(65).chr(255).chr(256),
  $subject
);

/* loop through to get the  $string */
for( $i = 0; $i < count($search_str); $i++ ) {
  echo "\n--- Iteration $i ---";
  echo "\n-- String after replacing the search value is => --\n";
  var_dump( str_replace($search_str[$i], "FOUND", $subject, $count) );
  echo "-- search string has found '$count' times\n";
}
  

echo "\n*** Testing Miscelleneous input data ***\n";
/*  If replace has fewer values than search, then an empty 
    string is used for the rest of replacement values */
var_dump( str_replace(array("a", "a", "b"), 
		      array("q", "q"), 
		      "aaabb", $count
		     )
	);
var_dump($count);
var_dump( str_replace(array("a", "a", "b"), 
                      array("q", "q"), 
                      array("aaa", "bbb", "ccc"), 
                      $count
                     )
        );
var_dump($count);


echo "\n-- Testing objects --\n";
/* we get "Catchable fatal error: saying Object of class could not be converted
        to string" by default, when an object is passed instead of string:
The error can be  avoided by chosing the __toString magix method as follows: */

class subject 
{
  function __toString() {
    return "Hello, world";
  }
}
$obj_subject = new subject;

class search 
{
  function __toString() {
    return "Hello, world";
  }
}
$obj_search = new search;

class replace 
{
  function __toString() {
    return "Hello, world";
  }
}
$obj_replace = new replace;

var_dump(str_replace("$obj_search", "$obj_replace", "$obj_subject", $count));
var_dump($count);


echo "\n-- Testing arrays --\n";
var_dump(str_replace(array("a", "a", "b"), "multi", "aaa", $count));
var_dump($count);

var_dump(str_replace( array("a", "a", "b"),
                      array("q", "q", "c"), 
                      "aaa", $count
                    )
);
var_dump($count);

var_dump(str_replace( array("a", "a", "b"),
                      array("q", "q", "c"), 
                      array("aaa", "bbb"), 
                      $count
                    )
);
var_dump($count);

var_dump(str_replace("a", array("q", "q", "c"), array("aaa", "bbb"), $count));
var_dump($count);

var_dump(str_replace("a", 1, array("aaa", "bbb"), $count));
var_dump($count);

var_dump(str_replace(1, 3, array("aaa1", "2bbb"), $count));
var_dump($count);


echo "\n-- Testing Resources --\n";
$resource1 = fopen( __FILE__, "r" );
$resource2 = opendir( "." );
var_dump(str_replace("stream", "FOUND", $resource1, $count)); 
var_dump($count);
var_dump(str_replace("stream", "FOUND", $resource2, $count));
var_dump($count);


echo "\n-- Testing a longer and heredoc string --\n";
$string = <<<EOD
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
@#$%^&**&^%$#@!~:())))((((&&&**%$###@@@!!!~~~~@###$%^&*
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;

var_dump( str_replace("abcdef", "FOUND", $string, $count) );
var_dump( $count );

echo "\n-- Testing a heredoc null string --\n";
$str = <<<EOD
EOD;
var_dump( str_replace("", "FOUND", $str, $count) );
var_dump( $count );


echo "\n-- Testing simple and complex syntax strings --\n";
$str = 'world';

/* Simple syntax */
var_dump( str_replace("world", "FOUND", "$str") );
var_dump( str_replace("world'S", "FOUND", "$str'S") );
var_dump( str_replace("worldS", "FOUND", "$strS") );

/* String with curly braces, complex syntax */
var_dump( str_replace("worldS", "FOUND", "${str}S") );
var_dump( str_replace("worldS", "FOUND", "{$str}S") );


echo "\n*** Testing error conditions ***";
/* Invalid arguments */
var_dump( str_replace() );
var_dump( str_replace("") );
var_dump( str_replace(NULL) );
var_dump( str_replace(1, 2) );
var_dump( str_replace(1,2,3,$var,5) );

echo "Done\n";

--CLEAN--
fclose($fp);
fclose($resource1);
closedir($resource2);

?>
--EXPECTF--	
*** Testing str_replace() on basic operations ***
string(0) ""
string(4) "tbst"
string(0) ""
int(0)
string(1) "q"
int(1)
string(0) ""
int(0)
string(14) "Resource id #5"
int(1)

*** Testing str_replace() with various search values ***
-- Iteration 0 --
array(12) {
  [0]=>
  string(5) "FOUND"
  [1]=>
  string(0) ""
  [2]=>
  string(5) "FOUND"
  [3]=>
  string(1) "0"
  [4]=>
  string(6) "-FOUND"
  [5]=>
  string(5) "FOUND"
  [6]=>
  string(1) "0"
  [7]=>
  string(6) "-FOUND"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(5)

-- Iteration 1 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(0)

-- Iteration 2 --
array(12) {
  [0]=>
  string(5) "FOUND"
  [1]=>
  string(0) ""
  [2]=>
  string(5) "FOUND"
  [3]=>
  string(1) "0"
  [4]=>
  string(6) "-FOUND"
  [5]=>
  string(5) "FOUND"
  [6]=>
  string(1) "0"
  [7]=>
  string(6) "-FOUND"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(5)

-- Iteration 3 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(5) "FOUND"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(5) "FOUND"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(2)

-- Iteration 4 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(5) "FOUND"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(5) "FOUND"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(2)

-- Iteration 5 --
array(12) {
  [0]=>
  string(5) "FOUND"
  [1]=>
  string(0) ""
  [2]=>
  string(5) "FOUND"
  [3]=>
  string(1) "0"
  [4]=>
  string(6) "-FOUND"
  [5]=>
  string(5) "FOUND"
  [6]=>
  string(1) "0"
  [7]=>
  string(6) "-FOUND"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(5)

-- Iteration 6 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(5) "FOUND"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(5) "FOUND"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(2)

-- Iteration 7 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(5) "FOUND"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(5) "FOUND"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(2)

-- Iteration 8 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(0)

-- Iteration 9 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(0)

-- Iteration 10 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(5) "FOUND"
  [11]=>
  string(0) ""
}
int(1)

-- Iteration 11 --
array(12) {
  [0]=>
  string(1) "1"
  [1]=>
  string(0) ""
  [2]=>
  string(1) "1"
  [3]=>
  string(1) "0"
  [4]=>
  string(2) "-1"
  [5]=>
  string(1) "1"
  [6]=>
  string(1) "0"
  [7]=>
  string(2) "-1"
  [8]=>
  string(0) ""
  [9]=>
  array(0) {
  }
  [10]=>
  string(3) "php"
  [11]=>
  string(0) ""
}
int(0)

*** Testing str_replace() with various subjects ***
--- Iteration 0 ---
-- String after replacing the search value is => --
string(177) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!FOUND 
	    ?FOUND chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 1 ---
-- String after replacing the search value is => --
string(177) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!FOUND 
	    ?FOUND chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 2 ---
-- String after replacing the search value is => --
string(182) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: FOUND 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 3 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 4 ---
-- String after replacing the search value is => --
string(182) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $FOUND: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 5 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 6 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 7 ---
-- String after replacing the search value is => --
string(189) "Hello, world,0120333.3445FOUND67          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 8 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 9 ---
-- String after replacing the search value is => --
string(193) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD FOUND \xXYZ	FOUND $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 10 ---
-- String after replacing the search value is => --
string(193) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xFOUND	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 11 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 12 ---
-- String after replacing the search value is => --
string(192) "Hello, world,0120333.3445-1.234567          FOUND TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 13 ---
-- String after replacing the search value is => --
string(207) "Hello, world,FOUND12FOUND333.3445-1.234567          NULL TRUE FALSE 
 	     FOUND«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(FOUND).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '4' times

--- Iteration 14 ---
-- String after replacing the search value is => --
string(207) "Hello, world,FOUND12FOUND333.3445-1.234567          NULL TRUE FALSE 
 	     FOUND«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(FOUND).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '4' times

--- Iteration 15 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 16 ---
-- String after replacing the search value is => --
string(307) "Hello,FOUNDworld,0120333.3445-1.234567FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDNULLFOUNDTRUEFOUNDFALSE 
FOUND	FOUNDFOUNDFOUNDFOUND 0«CD abcdFOUND\xXYZ	abcdFOUND$$@#%^&*!~,.:;?:FOUND!!Hello,FOUNDWorldFOUND
	FOUNDFOUNDFOUNDFOUND?Hello,FOUNDWorldFOUNDchr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '29' times

--- Iteration 17 ---
-- String after replacing the search value is => --
string(203) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSEFOUND
 	    FOUND0«CDFOUNDabcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 18 ---
-- String after replacing the search value is => --
string(194) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	    FOUND«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 19 ---
-- String after replacing the search value is => --
string(194) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0FOUNDD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 20 ---
-- String after replacing the search value is => --
string(194) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	    FOUND«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 21 ---
-- String after replacing the search value is => --
string(194) "Hello, world,0120333FOUND445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 22 ---
-- String after replacing the search value is => --
string(203) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 23 ---
-- String after replacing the search value is => --
string(192) "Hello, world,0120333.3445-1.234567          NULL FOUND FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 24 ---
-- String after replacing the search value is => --
string(203) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 25 ---
-- String after replacing the search value is => --
string(203) "Hello, world,0FOUND20333.3445-FOUND.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(FOUND28).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 26 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 27 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FOUND 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 28 ---
-- String after replacing the search value is => --
string(307) "Hello,FOUNDworld,0120333.3445-1.234567FOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDFOUNDNULLFOUNDTRUEFOUNDFALSE 
FOUND	FOUNDFOUNDFOUNDFOUND 0«CD abcdFOUND\xXYZ	abcdFOUND$$@#%^&*!~,.:;?:FOUND!!Hello,FOUNDWorldFOUND
	FOUNDFOUNDFOUNDFOUND?Hello,FOUNDWorldFOUNDchr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '29' times

--- Iteration 29 ---
-- String after replacing the search value is => --
string(186) "Hello, world,0120333.3445-1.234567FOUNDNULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '1' times

--- Iteration 30 ---
-- String after replacing the search value is => --
string(199) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD aFOUNDcd \xXYZ	aFOUNDcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '2' times

--- Iteration 31 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 32 ---
-- String after replacing the search value is => --
string(203) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 FOUND     0«CD abcd \xXYZFOUNDabcd $$@#%^&*!~,.:;?: !!Hello, World 
FOUND    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '3' times

--- Iteration 33 ---
-- String after replacing the search value is => --
string(191) "Hello, world,0120333.3445-1.234567          NULL TRUE FALSE 
 	     0«CD abcd \xXYZ	abcd $$@#%^&*!~,.:;?: !!Hello, World 
	    ?Hello, World chr(0).chr(128).chr(234).chr(65).chr(255).chr(256)"
-- search string has found '0' times

--- Iteration 34 ---
-- String after replacing the search value is => --
string(5) "FOUND"
-- search string has found '1' times

*** Testing Miscelleneous input data ***
string(3) "qqq"
int(5)
array(3) {
  [0]=>
  string(3) "qqq"
  [1]=>
  string(0) ""
  [2]=>
  string(3) "ccc"
}
int(6)

-- Testing objects --
string(12) "Hello, world"
int(1)

-- Testing arrays --
string(15) "multimultimulti"
int(3)
string(3) "qqq"
int(3)
array(2) {
  [0]=>
  string(3) "qqq"
  [1]=>
  string(3) "ccc"
}
int(6)

Notice: Array to string conversion in %s on line %d
array(2) {
  [0]=>
  string(15) "ArrayArrayArray"
  [1]=>
  string(3) "bbb"
}
int(3)
array(2) {
  [0]=>
  string(3) "111"
  [1]=>
  string(3) "bbb"
}
int(3)
array(2) {
  [0]=>
  string(4) "aaa3"
  [1]=>
  string(4) "2bbb"
}
int(1)

-- Testing Resources --
string(14) "Resource id #6"
int(0)
string(14) "Resource id #7"
int(0)

-- Testing a longer and heredoc string --
string(623) "FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789
@#$%^&**&^%$#@!~:())))((((&&&**%$###@@@!!!~~~~@###$%^&*
FOUNDghijklmnopqrstuvwxyz0123456789FOUNDghijklmnopqrstuvwxyz0123456789"
int(16)

-- Testing a heredoc null string --
string(0) ""
int(0)

-- Testing simple and complex syntax strings --
string(5) "FOUND"
string(5) "FOUND"

Notice: Undefined variable: strS in %s on line %d
string(0) ""
string(5) "FOUND"
string(5) "FOUND"

*** Testing error conditions ***
Warning: Wrong parameter count for str_replace() in %s on line %d
NULL

Warning: Wrong parameter count for str_replace() in %s on line %d
NULL

Warning: Wrong parameter count for str_replace() in %s on line %d
NULL

Warning: Wrong parameter count for str_replace() in %s on line %d
NULL

Warning: Wrong parameter count for str_replace() in %s on line %d
NULL
Done
