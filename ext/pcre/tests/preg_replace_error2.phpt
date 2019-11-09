--TEST--
Test preg_replace() function : error conditions - wrong arg types
--FILE--
<?php
/*
* proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
* Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_replace reacts to being passed the wrong type of replacement argument
*/
echo "*** Testing preg_replace() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$replace = array('this is a string', array('this is', 'a subarray'),);
$subject = 'test';
foreach($replace as $value) {
    @print "\nArg value is: $value\n";
    try {
        var_dump(preg_replace($regex, $value, $subject));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
$value = new stdclass(); //Object
try {
    var_dump(preg_replace($regex, $value, $subject));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo "Done";
?>
--EXPECTF--
*** Testing preg_replace() : error conditions ***

Arg value is: this is a string
string(64) "this is a stringthis is a stringthis is a stringthis is a string"

Arg value is: Array
Parameter mismatch, pattern is a string while replacement is an array
Object of class stdClass could not be converted to string
Done
