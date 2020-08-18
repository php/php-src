--TEST--
Test preg_replace() function : error - bad regular expressions
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_replace reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_replace() : error conditions***\n";
$regex_array = array('abcdef', //Regex without delimiter
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$replace = 1;
$subject = 'a';
foreach($regex_array as $regex_value) {
    @print "\nArg value is $regex_value\n";
    try {
        var_dump(preg_replace($regex_value, $replace, $subject));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
$regex_value = new stdclass(); //Object
try {
    preg_replace($regex_value, $replace, $subject);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
*** Testing preg_replace() : error conditions***

Arg value is abcdef
preg_replace(): Regular expression delimiter cannot be alphanumeric or a backslash

Arg value is /[a-zA-Z]
preg_replace(): Regular expression doesn't contain an ending delimiter "/"

Arg value is [a-zA-Z]/
preg_replace(): Regular expression modifier "/" is invalid

Arg value is /[a-zA-Z]/F
preg_replace(): Regular expression modifier "F" is invalid

Arg value is Array
string(1) "a"

Arg value is /[a-zA-Z]/
string(1) "1"
preg_replace(): Argument #1 ($regex) must be of type string|array, stdClass given
