--TEST--
Test preg_grep() function : error conditions - bad regular expressions
--FILE--
<?php
/*
* proto array preg_grep(string regex, array input [, int flags])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_grep reacts to being passed bad regexes
*/
echo "*** Testing preg_grep() : error conditions ***\n";
$values = array('abcdef', //Regex without delimeter
'/[a-zA-Z]', //Regex without closing delimeter
'[a-zA-Z]/', //Regex without opening delimeter
'/[a-zA-Z]/F', array('[a-z]', //Array of Regexes
'[A-Z]', '[0-9]'), '/[a-zA-Z]/', //Regex string
);
$array = array(123, 'abc', 'test');
foreach($values as $value) {
    print "\nArg value is $value\n";
    var_dump(preg_grep($value, $array));
}
$value = new stdclass(); //Object
var_dump(preg_grep($value, $array));
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

Warning: preg_grep() expects parameter 1 to be string, array given in %spreg_grep_error1.php on line %d
NULL

Arg value is /[a-zA-Z]/
array(2) {
  [1]=>
  string(3) "abc"
  [2]=>
  string(4) "test"
}

Warning: preg_grep() expects parameter 1 to be string, object given in %spreg_grep_error1.php on line %d
NULL
Done