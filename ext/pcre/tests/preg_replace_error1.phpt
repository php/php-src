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
$regex_array = [
    'abcdef', //Regex without delimiter
    '/[a-zA-Z]', //Regex without closing delimiter
    '[a-zA-Z]/', //Regex without opening delimiter
    '/[a-zA-Z]/F',
    [
        '[a-z]', //Array of Regexes
        '[A-Z]',
        '[0-9]',
    ],
    '/[a-zA-Z]/', //Regex string
];
$replace = 1;
$subject = 'a';
foreach ($regex_array as $regex_value) {
    var_dump(preg_replace($regex_value, $replace, $subject));
}
$regex_value = new stdclass(); //Object
try {
    var_dump(preg_replace($regex_value, $replace, $subject));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--

Warning: preg_replace(): Delimiter must not be alphanumeric, backslash, or NUL byte in %spreg_replace_error1.php on line %d
NULL

Warning: preg_replace(): No ending delimiter '/' found in %spreg_replace_error1.php on line %d
NULL

Warning: preg_replace(): Unknown modifier '/' in %spreg_replace_error1.php on line %d
NULL

Warning: preg_replace(): Unknown modifier 'F' in %spreg_replace_error1.php on line %d
NULL
string(1) "a"
string(1) "1"
preg_replace(): Argument #1 ($pattern) must be of type array|string, stdClass given
