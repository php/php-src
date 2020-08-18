--TEST--
Test preg_split() function : error conditions - bad regular expressions
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_split reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_split() : error conditions ***\n";
$regex_array = array('abcdef', //Regex without delimiter
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$subject = '1 2 a 3 4 b 5 6';
foreach($regex_array as $regex_value) {
    @print "\nArg value is $regex_value\n";
    try {
        var_dump(preg_split($regex_value, $subject));
    } catch (Error $exception) {
        echo get_class($exception) . ": " . $exception->getMessage() . "\n";
    }
}
$regex_value = new stdclass(); //Object
try {
    preg_split($regex_value, $subject);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing preg_split() : error conditions ***

Arg value is abcdef
ValueError: preg_split(): Regular expression delimiter cannot be alphanumeric or a backslash

Arg value is /[a-zA-Z]
ValueError: preg_split(): Regular expression doesn't contain an ending delimiter "/"

Arg value is [a-zA-Z]/
ValueError: preg_split(): Regular expression modifier "/" is invalid

Arg value is /[a-zA-Z]/F
ValueError: preg_split(): Regular expression modifier "F" is invalid

Arg value is Array
TypeError: preg_split(): Argument #1 ($pattern) must be of type string, array given

Arg value is /[a-zA-Z]/
array(3) {
  [0]=>
  string(4) "1 2 "
  [1]=>
  string(5) " 3 4 "
  [2]=>
  string(4) " 5 6"
}
preg_split(): Argument #1 ($pattern) must be of type string, stdClass given
