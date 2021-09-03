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
echo "*** Testing preg_grep() : error conditions ***\n";
$values = array('abcdef', //Regex without delimiter
'/[a-zA-Z]', //Regex without closing delimiter
'[a-zA-Z]/', //Regex without opening delimiter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$array = array(123, 'abc', 'test');
foreach($values as $value) {
    @print "\nArg value is $value\n";
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
echo "Done"
?>
--EXPECTF--
*** Testing preg_grep() : error conditions ***

Arg value is abcdef

Warning: preg_grep(): Delimiter must not be alphanumeric or backslash in %spreg_grep_error1.php on line %d
bool(false)

Arg value is /[a-zA-Z]

Warning: preg_grep(): No ending delimiter '/' found in %spreg_grep_error1.php on line %d
bool(false)

Arg value is [a-zA-Z]/

Warning: preg_grep(): Unknown modifier '/' in %spreg_grep_error1.php on line %d
bool(false)

Arg value is /[a-zA-Z]/F

Warning: preg_grep(): Unknown modifier 'F' in %spreg_grep_error1.php on line %d
bool(false)

Arg value is Array
preg_grep(): Argument #1 ($pattern) must be of type string, array given

Arg value is /[a-zA-Z]/
array(2) {
  [1]=>
  string(3) "abc"
  [2]=>
  string(4) "test"
}
preg_grep(): Argument #1 ($pattern) must be of type string, stdClass given
Done
