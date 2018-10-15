--TEST--
Test preg_replace() function : error conditions - wrong arg types
--FILE--
<?php
/*
* proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_replace reacts to being passed the wrong type of replacement argument
*/
echo "*** Testing preg_replace() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$replace = array('this is a string', array('this is', 'a subarray'),);
$subject = 'test';
foreach($replace as $value) {
    print "\nArg value is: $value\n";
    var_dump(preg_replace($regex, $value, $subject));
}
$value = new stdclass(); //Object
var_dump(preg_replace($regex, $value, $subject));
echo "Done";
?>
--EXPECTF--
*** Testing preg_replace() : error conditions ***

Arg value is: this is a string
string(64) "this is a stringthis is a stringthis is a stringthis is a string"

Arg value is: Array

Warning: preg_replace(): Parameter mismatch, pattern is a string while replacement is an array in %spreg_replace_error2.php on line %d
bool(false)

Recoverable fatal error: Object of class stdClass could not be converted to string in %spreg_replace_error2.php on line %d
