--TEST--
Test preg_match_all() function : error conditions - bad regular expressions
--FILE--
<?php
/*
 * Function is implemented in ext/pcre/php_pcre.c
 */
/*
 * Testing how preg_match_all reacts to being passed the wrong type of regex argument
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
$subject = 'test';
foreach ($regex_array as $regex_value) {
    try {
        var_dump(preg_match_all($regex_value, $subject, $matches1));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($matches1);
}
$regex_value = new stdclass(); //Object
try {
    var_dump(preg_match_all($regex_value, $subject, $matches));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($matches);
?>
--EXPECTF--

Warning: preg_match_all(): Delimiter must not be alphanumeric, backslash, or NUL byte in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Warning: preg_match_all(): No ending delimiter '/' found in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Warning: preg_match_all(): Unknown modifier '/' in %spreg_match_all_error1.php on line %d
bool(false)
NULL

Warning: preg_match_all(): Unknown modifier 'F' in %spreg_match_all_error1.php on line %d
bool(false)
NULL
preg_match_all(): Argument #1 ($pattern) must be of type string, array given
NULL
int(4)
array(1) {
  [0]=>
  array(4) {
    [0]=>
    string(1) "t"
    [1]=>
    string(1) "e"
    [2]=>
    string(1) "s"
    [3]=>
    string(1) "t"
  }
}
preg_match_all(): Argument #1 ($pattern) must be of type string, stdClass given
NULL
