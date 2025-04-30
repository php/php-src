--TEST--
Test preg_grep() function : error conditions - bad regular expressions
--FILE--
<?php
/*
 * Function is implemented in ext/pcre/php_pcre.c
 */
/*
 * Testing how preg_grep reacts to being passed bad regexes
 */
$values = [
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
$array = [123, 'abc', 'test'];
foreach ($values as $value) {
    try {
        var_dump(preg_grep($value, $array));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
$value = new stdclass(); //Object
try {
    var_dump(preg_grep($value, $array));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--

Warning: preg_grep(): Delimiter must not be alphanumeric, backslash, or NUL byte in %spreg_grep_error1.php on line %d
bool(false)

Warning: preg_grep(): No ending delimiter '/' found in %spreg_grep_error1.php on line %d
bool(false)

Warning: preg_grep(): Unknown modifier '/' in %spreg_grep_error1.php on line %d
bool(false)

Warning: preg_grep(): Unknown modifier 'F' in %spreg_grep_error1.php on line %d
bool(false)
preg_grep(): Argument #1 ($pattern) must be of type string, array given
array(2) {
  [1]=>
  string(3) "abc"
  [2]=>
  string(4) "test"
}
preg_grep(): Argument #1 ($pattern) must be of type string, stdClass given
