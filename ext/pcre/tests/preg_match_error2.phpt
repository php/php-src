--TEST--
Test preg_match() function : error conditions - wrong arg types
--FILE--
<?php
/*
 *  proto int preg_match(string pattern, string subject [, array subpatterns [, int flags [, int offset]]])
 * Function is implemented in ext/pcre/php_pcre.c
*/
/*
* Testing how preg_match reacts to being passed the wrong type of subject argument
*/
echo "*** Testing preg_match() : error conditions ***\n";
$regex = '/[a-zA-Z]/';
$input = array('this is a string', array('this is', 'a subarray'),);
foreach($input as $value) {
    @print "\nArg value is: $value\n";
    try {
        var_dump(preg_match($regex, $value));
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
$value = new stdclass(); //Object
try {
    var_dump(preg_match($regex, $value));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
echo "Done";
?>
--EXPECTF--
*** Testing preg_match() : error conditions ***

Arg value is: this is a string
int(1)

Arg value is: Array
preg_match() expects parameter 2 to be string, array given
preg_match() expects parameter 2 to be string, object given
Done
