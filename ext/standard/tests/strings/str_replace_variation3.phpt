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
The error can be  avoided by choosing the __toString magix method as follows: */

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

var_dump(str_replace("a", array("q", "q", "c"), array("aaa"), $count));
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


fclose($resource1);
closedir($resource2);

?>
===DONE===
--EXPECTF--	
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
array(1) {
  [0]=>
  string(15) "ArrayArrayArray"
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
string(%d) "Resource id #%d"
int(0)
string(%d) "Resource id #%d"
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
===DONE===
