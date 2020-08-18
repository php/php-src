--TEST--
Test preg_replace_callback() function : error
--FILE--
<?php
/*
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_replace_callback reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_replace_callback() : error conditions ***\n";
$regex_array = array('abcdef', //Regex without delimiters
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[0-9]/'); //Regex string
$replacement = array('zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine');
function integer_word($matches) {
    global $replacement;
    return $replacement[$matches[0]];
}
$subject = 'number 1.';
foreach($regex_array as $regex_value) {
    @print "\nArg value is $regex_value\n";
    try {
        var_dump(preg_replace_callback($regex_value, 'integer_word', $subject));
    } catch (Error $exception) {
        echo get_class($exception) . ": " . $exception->getMessage() . "\n";
    }
}
?>
--EXPECT--
*** Testing preg_replace_callback() : error conditions ***

Arg value is abcdef
ValueError: preg_replace_callback(): Regular expression delimiter cannot be alphanumeric or a backslash

Arg value is /[a-zA-Z]
ValueError: preg_replace_callback(): Regular expression doesn't contain an ending delimiter "/"

Arg value is [a-zA-Z]/
ValueError: preg_replace_callback(): Regular expression modifier "/" is invalid

Arg value is /[a-zA-Z]/F
ValueError: preg_replace_callback(): Regular expression modifier "F" is invalid

Arg value is Array
string(9) "number 1."

Arg value is /[0-9]/
string(11) "number one."
