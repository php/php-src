--TEST--
Test preg_split() function : error conditions - wrong arg types
--FILE--
<?php
/*
* proto array preg_split(string pattern, string subject [, int limit [, int flags]])
* Function is implemented in ext/pcre/php_pcre.c
*/
error_reporting(E_ALL&~E_NOTICE);
/*
* Testing how preg_split reacts to being passed the wrong type of input argument
*/
echo "*** Testing preg_split() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$input = array(array('this is', 'a subarray'),);
foreach($input as $value) {
    print "\nArg value is: $value\n";
    var_dump(preg_split($regex, $value));
}
$value = new stdclass(); //Object
var_dump(preg_split($regex, $value));
echo "Done";
?>
--EXPECTF--
*** Testing preg_split() : error conditions ***

Arg value is: Array

Warning: preg_split() expects parameter 2 to be string, array given in %spreg_split_error2.php on line %d
bool(false)

Warning: preg_split() expects parameter 2 to be string, object given in %spreg_split_error2.php on line %d
bool(false)
Done
