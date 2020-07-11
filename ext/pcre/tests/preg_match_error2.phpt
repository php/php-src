--TEST--
Test preg_match() function : error conditions - wrong arg types
--FILE--
<?php
/* Function is implemented in ext/pcre/php_pcre.c */
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
--EXPECT--
*** Testing preg_match() : error conditions ***

Arg value is: this is a string
int(1)

Arg value is: Array
preg_match(): Argument #2 ($subject) must be of type string, array given
preg_match(): Argument #2 ($subject) must be of type string, stdClass given
Done
