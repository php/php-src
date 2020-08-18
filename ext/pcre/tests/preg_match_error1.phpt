--TEST--
Test preg_match() function : error conditions - bad regular expressions
--FILE--
<?php
/* Function is implemented in ext/pcre/php_pcre.c */
/*
* Testing how preg_match reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_match() : error conditions ***\n";
$regex_array = array('abcdef', //Regex without delimiter
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$subject = 'this is a test';
foreach($regex_array as $regex_value) {
    @print "\nArg value is $regex_value\n";
    try {
        var_dump(preg_match($regex_value, $subject));
    } catch (Error $e) {
        echo get_class($e) . ": " . $e->getMessage(), "\n";
    }
}
$regex_value = new stdclass(); //Object
try {
    preg_match($regex_value, $subject);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
*** Testing preg_match() : error conditions ***

Arg value is abcdef
ValueError: preg_match(): Regular expression delimiter cannot be alphanumeric or a backslash

Arg value is /[a-zA-Z]
ValueError: preg_match(): Regular expression doesn't contain an ending delimiter "/"

Arg value is [a-zA-Z]/
ValueError: preg_match(): Regular expression modifier "/" is invalid

Arg value is /[a-zA-Z]/F
ValueError: preg_match(): Regular expression modifier "F" is invalid

Arg value is Array
TypeError: preg_match(): Argument #1 ($pattern) must be of type string, array given

Arg value is /[a-zA-Z]/
int(1)
preg_match(): Argument #1 ($pattern) must be of type string, stdClass given
