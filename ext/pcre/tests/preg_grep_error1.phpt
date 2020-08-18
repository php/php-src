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
    } catch (Error $e) {
        echo get_class($e) . ": " . $e->getMessage(), "\n";
    }
}
$value = new stdclass(); //Object
try {
    preg_grep($value, $array);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
echo "Done"
?>
--EXPECT--
*** Testing preg_grep() : error conditions ***

Arg value is abcdef
ValueError: preg_grep(): Regular expression delimiter cannot be alphanumeric or a backslash

Arg value is /[a-zA-Z]
ValueError: preg_grep(): Regular expression doesn't contain an ending delimiter "/"

Arg value is [a-zA-Z]/
ValueError: preg_grep(): Regular expression modifier "/" is invalid

Arg value is /[a-zA-Z]/F
ValueError: preg_grep(): Regular expression modifier "F" is invalid

Arg value is Array
TypeError: preg_grep(): Argument #1 ($regex) must be of type string, array given

Arg value is /[a-zA-Z]/
array(2) {
  [1]=>
  string(3) "abc"
  [2]=>
  string(4) "test"
}
preg_grep(): Argument #1 ($regex) must be of type string, stdClass given
Done
