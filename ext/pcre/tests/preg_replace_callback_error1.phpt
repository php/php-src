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
$regex_array = [
    'abcdef', //Regex without delimiters
    '/[a-zA-Z]', //Regex without closing delimiter
    '[a-zA-Z]/', //Regex without opening delimiter
    '/[a-zA-Z]/F',
    [
        '[a-z]', //Array of Regexes
        '[A-Z]',
        '[0-9]',
    ],
    '/[0-9]/', //Regex string
];
$replacement = [
    'zero',
    'one',
    'two',
    'three',
    'four',
    'five',
    'six',
    'seven',
    'eight',
    'nine',
];
function integer_word($matches) {
    global $replacement;
    return $replacement[$matches[0]];
}
$subject = 'number 1.';
foreach ($regex_array as $regex_value) {
    var_dump(preg_replace_callback($regex_value, 'integer_word', $subject));
}
?>
--EXPECTF--

Warning: preg_replace_callback(): Delimiter must not be alphanumeric, backslash, or NUL byte in %s on line %d
NULL

Warning: preg_replace_callback(): No ending delimiter '/' found in %s on line %d
NULL

Warning: preg_replace_callback(): Unknown modifier '/' in %s on line %d
NULL

Warning: preg_replace_callback(): Unknown modifier 'F' in %s on line %d
NULL
string(9) "number 1."
string(11) "number one."
