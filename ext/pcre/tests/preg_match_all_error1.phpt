--TEST--
Test preg_match_all() function : error conditions - bad regular expressions
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_match_all reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_match_all() : error conditions ***\n";
$regex_array = array('abcdef', //Regex without delimiter
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$subject = 'test';
foreach($regex_array as $regex_value) {
    @print "\nArg value is $regex_value\n";
    try {
        var_dump(preg_match_all($regex_value, $subject, $matches1));
    } catch (Error $e) {
        echo get_class($e) . ": " . $e->getMessage(), "\n";
    }
    var_dump($matches1);
}
$regex_value = new stdclass(); //Object
try {
    preg_match_all($regex_value, $subject, $matches);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($matches);
?>
--EXPECT--
*** Testing preg_match_all() : error conditions ***

Arg value is abcdef
ValueError: preg_match_all(): Regular expression delimiter cannot be alphanumeric or a backslash
NULL

Arg value is /[a-zA-Z]
ValueError: preg_match_all(): Regular expression doesn't contain an ending delimiter "/"
NULL

Arg value is [a-zA-Z]/
ValueError: preg_match_all(): Regular expression modifier "/" is invalid
NULL

Arg value is /[a-zA-Z]/F
ValueError: preg_match_all(): Regular expression modifier "F" is invalid
NULL

Arg value is Array
TypeError: preg_match_all(): Argument #1 ($pattern) must be of type string, array given
NULL

Arg value is /[a-zA-Z]/
int(4)
array(1) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "t"
    [1]=>
    string(1) "e"
    [2]=>
    string(1) "s"
    [3]=>
    string(1) "t"
  }
}
preg_match_all(): Argument #1 ($pattern) must be of type string, stdClass given
NULL
