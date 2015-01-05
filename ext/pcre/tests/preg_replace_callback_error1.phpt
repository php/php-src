--TEST--
Test preg_replace_callback() function : error 
--FILE--
<?php
/*
* proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_replace_callback reacts to being passed the wrong type of regex argument
*/
echo "*** Testing preg_replace_callback() : error conditions ***\n";
$regex_array = array('abcdef', //Regex without delimiters
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/'); //Regex string
$replacement = array('zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine');
function integer_word($matches) {
    global $replacement;
    return $replacement[$matches[0]];
}
$subject = 'number 1.';
foreach($regex_array as $regex_value) {
    print "\nArg value is $regex_value\n";
    var_dump(preg_replace_callback($regex_value, 'integer_word', $subject));
}
?>
===Done===
--EXPECTF--
*** Testing preg_replace_callback() : error conditions ***

Arg value is abcdef

Warning: preg_replace_callback(): Delimiter must not be alphanumeric or backslash in %s on line %d
NULL

Arg value is /[a-zA-Z]

Warning: preg_replace_callback(): No ending delimiter '/' found in %s on line %d
NULL

Arg value is [a-zA-Z]/

Warning: preg_replace_callback(): Unknown modifier '/' in %s on line %d
NULL

Arg value is /[a-zA-Z]/F

Warning: preg_replace_callback(): Unknown modifier 'F' in %s on line %d
NULL

Arg value is Array
string(9) "number 1."

Arg value is /[a-zA-Z]/
string(3) " 1."
===Done===