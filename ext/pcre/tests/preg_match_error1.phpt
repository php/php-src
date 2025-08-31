--TEST--
Test preg_match() function : error conditions - bad regular expressions
--FILE--
<?php
/* Function is implemented in ext/pcre/php_pcre.c */
/*
 * Testing how preg_match reacts to being passed the wrong type of regex argument
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
$subject = 'this is a test';
foreach ($regex_array as $regex_value) {
    try {
        var_dump(preg_match($regex_value, $subject));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
$regex_value = new stdclass(); //Object
try {
    var_dump(preg_match($regex_value, $subject));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--

Warning: preg_match(): Delimiter must not be alphanumeric, backslash, or NUL byte in %spreg_match_error1.php on line %d
bool(false)

Warning: preg_match(): No ending delimiter '/' found in %spreg_match_error1.php on line %d
bool(false)

Warning: preg_match(): Unknown modifier '/' in %spreg_match_error1.php on line %d
bool(false)

Warning: preg_match(): Unknown modifier 'F' in %spreg_match_error1.php on line %d
bool(false)
preg_match(): Argument #1 ($pattern) must be of type string, array given
int(1)
preg_match(): Argument #1 ($pattern) must be of type string, stdClass given
