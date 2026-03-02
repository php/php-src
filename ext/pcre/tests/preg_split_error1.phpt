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
$subject = '1 2 a 3 4 b 5 6';
foreach ($regex_array as $regex_value) {
    try {
        var_dump(preg_split($regex_value, $subject));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
$regex_value = new stdclass(); //Object
try {
    var_dump(preg_split($regex_value, $subject));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--

Warning: preg_split(): Delimiter must not be alphanumeric, backslash, or NUL byte in %spreg_split_error1.php on line %d
bool(false)

Warning: preg_split(): No ending delimiter '/' found in %spreg_split_error1.php on line %d
bool(false)

Warning: preg_split(): Unknown modifier '/' in %spreg_split_error1.php on line %d
bool(false)

Warning: preg_split(): Unknown modifier 'F' in %spreg_split_error1.php on line %d
bool(false)
preg_split(): Argument #1 ($pattern) must be of type string, array given
array(3) {
  [0]=>
  string(4) "1 2 "
  [1]=>
  string(5) " 3 4 "
  [2]=>
  string(4) " 5 6"
}
preg_split(): Argument #1 ($pattern) must be of type string, stdClass given
